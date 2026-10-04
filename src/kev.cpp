#include "kev.hpp"
#include <cpr/cpr.h>
#include "globals.hpp"

void CKev::request_decision( const std::string& state, const std::vector<Direction>& allowed ) {
    std::scoped_lock lock( m_mutex );
    m_pending_state = state;
    m_allowed_directions = allowed;
    m_thinking = true;
    m_cv.notify_one( );
}

// private
std::string CKev::explain_legend() const {
    std::string str{ "Symbol 'R' means 'robot', which is you.\nSymbol 'V' means 'visited', an already visited open cell.\n" };

    for (const auto& entry : legend) {
        str += std::format("Symbol '{}' means '{}'\n", entry.symbol, entry.meaning);
    }

    return str;
}

json CKev::build_request( const std::string& state, const std::vector<Direction>& allowed) {
    json criteria = json::object();

    auto str = std::format("You control a robot moving around a small map, your goal is to get to cell 'T'. If no T is visible move to an open cell, preferring unvisited ones and never #. Here is a legend to help you navigate: {}\nIf T is visible, answer with the direction toward it. Otherwise, pick the direction of an unvisited open cell.", explain_legend());

    for (const auto ad : allowed) {
        criteria[reverse_lookup.at(ad)] = nullptr;
    }
    if (criteria.empty()) {
        for (const auto& [dir, name] : reverse_lookup) criteria[name] = nullptr;
    }

//#ifndef NDEBUG
//    std::println("prompt: {}", str);
//#endif

    return {
        { "state", state },
        { "model", "kev-latest" },
        { "questions",
          { { "direction",
              { { "type", "choice" },
                { "instructions", str },
                { "criteria",
                  criteria }}}}} };
}

void CKev::worker_loop( ) {
    while ( true ) {
        std::unique_lock lock( m_mutex );
        m_cv.wait( lock, [this] { return !m_running || !m_pending_state.empty( ); } );

        if ( !m_running ) break;
        ThinkingGuard guard(m_thinking);

        std::string state = std::move( m_pending_state );
        m_pending_state.clear( );
        std::vector<Direction> allowed_directions = std::move(m_allowed_directions);
        m_allowed_directions.clear();
        lock.unlock( );

        Decision decision;

        try {
            json data = build_request(state, allowed_directions);
            auto now = std::chrono::steady_clock::now();

            cpr::Response r = cpr::Post(
                cpr::Url{ g_endpoint }, cpr::Body{ data.dump() }, cpr::Header{ { "content-type", "application/json" } }, cpr::Timeout(10000));

            auto err = r.error;
            if (err) {
                std::println("[CKev] an error({}) occured: {}", static_cast<int>(err.code), err.message);
                continue;
            }

            auto request_duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - now).count();
#ifndef NDEBUG
            std::println("[DEBUG]: request took: {}ms", request_duration);
#endif

            data = json::parse( r.text );

            double confidence = data.at("answers").at("direction").at("confidence");
            std::string direction = data.at("answers").at("direction").at("choice");

#ifndef NDEBUG
                std::println("[DEBUG]: confidence level: {:.2f}", confidence);
#endif
            auto it = lookup.find(direction);
            if (it == lookup.end()) {
                //this can theoretically never happen with how Jev/Kev works
                continue;
            }

            decision.confidence = confidence;
            decision.direction = it->second;
        }
        catch (const json::exception& err) {
            std::println("[Kev] an error occured whilst parsing the response from Kev: {}", err.what());
            continue;
        }

        lock.lock( );
        m_result = decision;
    }
}
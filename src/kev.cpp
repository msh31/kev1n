#include "kev.hpp"
#include <cpr/cpr.h>
#include "globals.hpp"

constexpr std::string_view endpoint{ "http://127.0.0.1:8009/v1/systemone" };

void CKev::request_decision( const std::string& state ) {
    std::scoped_lock lock( m_mutex );
    m_pending_state = state;
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

json CKev::build_request( const std::string& state ) {
    auto str = std::format("You control a robot moving around a small map, your goal is to get to cell 'T'. If no T is visible move to an open cell, preferring unvisited ones and never #. Here is a legend to help you navigate: {}\nAnswer with the direction from R toward T", explain_legend());

#ifndef NDEBUG
    std::println("prompt: {}", str);
#endif

    return {
        { "state", state },
        { "model", "kev-latest" },
        { "questions",
          { { "direction",
              { { "type", "choice" },
                { "instructions", str },
                { "criteria",
                  { { "up", nullptr }, { "down", nullptr }, { "left", nullptr }, { "right", nullptr } } } } } } } };
}

void CKev::worker_loop( ) {
    while ( true ) {
        std::unique_lock lock( m_mutex );
        m_cv.wait( lock, [this] { return !m_running || !m_pending_state.empty( ); } );

        if ( !m_running ) break;
        ThinkingGuard guard(m_thinking);

        std::string state = std::move( m_pending_state );
        m_pending_state.clear( );
        lock.unlock( );

        Decision decision;

        try {
            json data = build_request(state);
            cpr::Response r = cpr::Post(
                cpr::Url{ endpoint }, cpr::Body{ data.dump() }, cpr::Header{ { "content-type", "application/json" } });

            data = json::parse( r.text );

            double confidence = data.at("answers").at("direction").at("confidence");
            std::string direction = data.at("answers").at("direction").at("choice");

            bool under_confidence_threshold = confidence <= g_confidence_threshold;
            if (under_confidence_threshold) {
#ifndef NDEBUG
                std::println("[DEBUG]: choice '{}' was rejected because its confidence level is too low: {:.2f}", direction, confidence);
#endif
                continue;
            }
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
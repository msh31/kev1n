#include "kev.hpp"
#include <cpr/cpr.h>

constexpr std::string_view endpoint{ "http://127.0.0.1:8009/v1/systemone" };

void CKev::request_decision( const std::string& state ) {
    std::scoped_lock lock( m_mutex );
    m_pending_state = state;
    m_cv.notify_one( );
}

// private
json CKev::build_request( const std::string& state ) {
    return {
        { "state", state },
        { "model", "kev-latest" },
        { "questions",
          { { "direction",
              { { "type", "choice" },
                { "instructions", "Which direction should the robot move?" },
                { "criteria",
                  { { "up", nullptr }, { "down", nullptr }, { "left", nullptr }, { "right", nullptr } } } } } } } };
}

void CKev::worker_loop( ) {
    while ( true ) {
        std::unique_lock lock( m_mutex );
        m_cv.wait( lock, [this] { return !m_running || !m_pending_state.empty( ); } );

        if ( !m_running ) break;

        std::string state = std::move( m_pending_state );
        m_pending_state.clear( );
        lock.unlock( );

        json data = build_request( state );
        cpr::Response r = cpr::Post(
            cpr::Url{ endpoint }, cpr::Body{ data.dump( ) }, cpr::Header{ { "content-type", "application/json" } } );

        // TODO: guard
        data = json::parse( r.text );

        lock.lock( );
        // m_result = decision; //TODO build this
    }
}
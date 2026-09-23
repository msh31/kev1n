#pragma once
#include "types.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

static const std::unordered_map<std::string, Direction> lookup = {
    { "up", Direction::UP }, { "down", Direction::DOWN }, { "left", Direction::LEFT }, { "right", Direction::RIGHT } };

struct Decision {
        Direction direction;
        double confidence;
};

class CKev {
    public:
        CKev( ) {
            m_running = true;
            m_worker = std::thread( &CKev::worker_loop, this );
        }

        ~CKev( ) {
            m_running = false;
            m_cv.notify_one( );
            if ( m_worker.joinable( ) ) m_worker.join( );
        }

        void request_decision( const std::string& state );

        std::optional<Decision> try_get_decision( ) {
            std::scoped_lock lock( m_mutex );
            if ( !m_result ) return std::nullopt;
            auto res = m_result;
            m_result = std::nullopt;
            return res;
        }

    private:
        std::string m_pending_state{ };
        std::optional<Decision> m_result = std::nullopt;

        std::thread m_worker;
        std::atomic<bool> m_running = false;

        std::mutex m_mutex;
        std::condition_variable m_cv;

        json build_request( const std::string& state );
        void worker_loop( );
};
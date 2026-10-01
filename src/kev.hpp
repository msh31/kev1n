#pragma once
#include "types.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

static const std::unordered_map<std::string, Direction> lookup = {
    { "up", Direction::UP }, { "down", Direction::DOWN }, { "left", Direction::LEFT }, { "right", Direction::RIGHT } };

static const std::array<ItemSymbol, 4> legend = {
    ItemSymbol{ '.', "open" }, //NONE
    ItemSymbol{ '#', "blocked" }, //OBSTACLE
    ItemSymbol{ 'T', "target" }, //TARGET
    ItemSymbol{ 'S', "station" }, //STATION
};

struct Decision {
        Direction direction;
        double confidence;
};

struct ThinkingGuard {
    ThinkingGuard(std::atomic<bool>& thinking) : m_thinking(thinking) {};
    ~ThinkingGuard() {
        m_thinking = false;
    }

    ThinkingGuard(const ThinkingGuard&) = delete;
    ThinkingGuard& operator=(const ThinkingGuard&) = delete;
private:
    std::atomic<bool>& m_thinking;
};

class CKev {
    public:
        CKev( ) {
            m_running = true;
            m_worker = std::thread( &CKev::worker_loop, this );
        }

        ~CKev( ) {
            { std::scoped_lock lock(m_mutex); m_running = false; }
            m_cv.notify_one();
            if ( m_worker.joinable( ) ) m_worker.join( );
        }

        void request_decision( const std::string& state );

        bool is_thinking() const { return m_thinking; }

        std::optional<Decision> try_get_decision( ) {
            std::scoped_lock lock( m_mutex );
            if ( !m_result ) return std::nullopt;
            auto res = m_result;
            m_result = std::nullopt;
            return res;
        }

        CKev(const CKev&) = delete;
        CKev& operator=(const CKev&) = delete;
        CKev(CKev&&) = delete;
        CKev& operator=(CKev&&) = delete;

    private:
        std::string m_pending_state{ };
        std::optional<Decision> m_result = std::nullopt;

        std::thread m_worker;
        std::atomic<bool> m_running = false;
        std::atomic<bool> m_thinking = false;

        std::mutex m_mutex;
        std::condition_variable m_cv;

        inline std::string explain_legend() const;
        json build_request( const std::string& state );
        void worker_loop( );
};
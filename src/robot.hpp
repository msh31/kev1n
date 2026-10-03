#pragma once
#include "types.hpp"
#include "world.hpp"
#include "kev.hpp"

constexpr int visited_cell_count = g_world_size * g_world_size;

class CRobot {
    public:
        CRobot( int spawn_x, int spawn_y, const CWorld& world ) : m_world( world ) {
            if ( !set_pos( spawn_x, spawn_y ) ) {
                throw std::runtime_error( "[CRobot] could not set initial spawn location!" );
            }
        }

        void update();
        void draw( Color color );

        [[nodiscard]]
        bool move( Direction direction ); //TODO: rename this lol

        bool reached_destination = false;

        std::pair<int, int> get_pos( ) { return { m_pos_x, m_pos_y }; }

    private:
        int m_pos_x = -1;
        int m_pos_y = -1;

        [[nodiscard]]
        bool set_pos( int x, int y );
        [[nodiscard]]
        bool would_move_succeed(int x, int y) const;

        [[nodiscard]]
        std::string build_state(int radius = 3);

        const CWorld& m_world;
        CKev m_kev;

        std::array<bool, visited_cell_count> m_visited_cells{};

        std::pair<int, int> get_adjacent_pos(Direction direction) const;
        std::vector<Direction> allowed_directions() const;
};
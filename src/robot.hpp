#pragma once
#include "types.hpp"
#include "world.hpp"

constexpr int g_robot_size = 40;

class CRobot {
    public:
        CRobot( int spawn_x, int spawn_y, const CWorld& world ) : m_world( world ) {
            if ( !set_pos( spawn_x, spawn_y ) ) {
                throw std::runtime_error( "[CRobot] could not set initial spawn location!" );
            }
        }

        void draw( Color color );

        bool move( Direction direction );

        std::pair<int, int> get_pos( ) { return { m_pos_x, m_pos_y }; }

    private:
        int m_pos_x = -1;
        int m_pos_y = -1;

        [[nodiscard]]
        bool set_pos( int x, int y );

        const CWorld& m_world;
};
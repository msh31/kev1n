#include "world.hpp"

// #ifdef __APPLE__
//     #include <raylib.h>
// #endif

void CWorld::spawn_item( int x, int y, float size, ItemType item ) {
    auto index = y * g_world_size + x;

    if ( x < 0 || x >= g_world_size || y < 0 || y >= g_world_size ) {
        std::println( "[CWorld]: spawn index is out of bounds!" );
        return;
    }

    m_cells[index] = Cell{ size, item };
}

bool CWorld::is_cell_blocked( int x, int y ) const {
    auto index = y * g_world_size + x;

    if ( x < 0 || x >= g_world_size || y < 0 || y >= g_world_size ) {
        std::println( "[CWorld]: requested cell index is out of bounds!" );
        return true;
    }

    if ( m_cells[index].type == ItemType::NONE ) return false;
    return true;
}

// this is stolen from SaveManager you can find it here ->
// https://github.com/msh31/SaveManager/blob/b918bd9c6f7cea3397eeebe266270e6b98dd8e62/lib/include/utils/utils.hpp#L132
template <typename Range, typename Fn> void enumerate( Range& range, Fn fn ) {
#ifdef __APPLE__
    int i = 0;
    for ( auto& r : range ) {
        fn( i, r );
        ++i;
    }
#else
    for ( auto [i, element] : std::views::enumerate( range ) ) {
        fn( i, element );
    }
#endif
}

void CWorld::render( ) {
    enumerate( m_cells, [this]( auto i, auto& c ) {
        if ( c.type == ItemType::NONE ) return;

        int x = i % g_world_size;
        int y = i / g_world_size;

        Color color{ };

        switch ( c.type ) {
        case ItemType::OBSTACLE: {
            color = RED;
        } break;

        case ItemType::TARGET: {
            color = BLUE;
        } break;

        case ItemType::STATION: {
            color = YELLOW;
        } break;

        default:
        case ItemType::NONE: {
        } break;
        }

        DrawRectangle(
            x * g_cell_pixel_size, y * g_cell_pixel_size, static_cast<int>( c.size ), static_cast<int>( c.size ),
            color );
    } );
    DrawRectangleLines( 0, 0, g_world_size * g_cell_pixel_size, g_world_size * g_cell_pixel_size, RED );
}

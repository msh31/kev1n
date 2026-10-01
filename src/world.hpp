#pragma once
#include "globals.hpp"

enum class ItemType { NONE, OBSTACLE, TARGET, STATION };

struct Cell {
        float size;

        ItemType type;
};

class CWorld {
    public:
        void render( );
        void spawn_item( int x, int y, float size, ItemType item );

        bool is_cell_blocked( int x, int y ) const;
        ItemType get_cell_type(int x, int y) const;

    private:
        std::array<Cell, g_world_size * g_world_size> m_cells;
};
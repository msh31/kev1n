#pragma once

constexpr int g_world_size = 512;
constexpr int g_cell_pixel_size = 20;

enum class ItemType { NONE, OBSTACLE, TARGET, STATION };

struct Cell {
	float size;

	ItemType type;
};

class CWorld {
public:
	void render();
	void spawn_item(int x, int y, float size, ItemType item);
	bool is_cell_blocked(int x, int y) const;

private:
	std::array<Cell, g_world_size * g_world_size> m_cells;
};
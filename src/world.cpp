#include "world.hpp"

void CWorld::spawn_item(int x, int y, float size, ItemType item) {
	auto index = y * g_world_size + x;

	if (x < 0 || x >= g_world_size || y < 0 || y >= g_world_size) {
		std::println("[CWorld]: spawn index is out of bounds!");
		return;
	}

	m_cells[index] = Cell{ size, item };
}

bool CWorld::is_cell_blocked(int x, int y) const {
	auto index = y * g_world_size + x;

	if (x < 0 || x >= g_world_size || y < 0 || y >= g_world_size) {
		std::println("[CWorld]: requested cell index is out of bounds!");
		return true;
	}

	if (m_cells[index].type == ItemType::NONE) return false;
	return true;
}

void CWorld::render() {
	rlPushMatrix();
	rlTranslatef(0, 25 * 50, 0);
	rlRotatef(90, 1, 0, 0);
	DrawGrid(g_world_size, g_cell_pixel_size);
	rlPopMatrix();

	for (auto [i, cell] : m_cells | std::views::enumerate) {
		if (cell.type == ItemType::NONE) continue;
		int x = i % g_world_size;
		int y = i / g_world_size;
		
		Color color{};

		switch (cell.type) {
			case ItemType::OBSTACLE: {
				color = RED;
			}
			break;

			case ItemType::TARGET: {
				color = BLUE;
			}
			break;

			case ItemType::STATION: {
				color = YELLOW;
			}
			break;
		}

		DrawRectangle(x * g_cell_pixel_size, y * g_cell_pixel_size, static_cast<int>(cell.size), static_cast<int>(cell.size), color);
	}
}
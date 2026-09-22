#include "robot.hpp"
#include "globals.hpp"

void CRobot::draw(Color color) {
	DrawRectangle(m_pos_x * g_cell_pixel_size, m_pos_y * g_cell_pixel_size, g_robot_size, g_robot_size, color);
}

bool CRobot::move(Direction direction) {
	auto c_pos = get_pos();

	int x{ c_pos.first }, y{ c_pos.second };

	switch (direction) {
		case Direction::UP: {
			y -= 1;
		}
		break;
		case Direction::DOWN: {
			y += 1;
		}
		break;
		case Direction::LEFT: {
			x -= 1;
		}
		break;
		case Direction::RIGHT: {
			x += 1;
		}
		break;
	}

	return set_pos(x, y);
}

//private
bool CRobot::set_pos(int x, int y) {
	if (m_world.is_cell_blocked(x, y)) {
		return false;
	}

	m_pos_x = x;
	m_pos_y = y;
	return true;
}
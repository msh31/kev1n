#include "robot.hpp"
#include "globals.hpp"

void CRobot::draw( Color color ) {
    DrawRectangle( m_pos_x * g_cell_pixel_size, m_pos_y * g_cell_pixel_size, g_cell_pixel_size, g_cell_pixel_size, color );
}

bool CRobot::move( Direction direction ) {
    std::pair<int, int> pos = get_adjacent_pos(direction);

    auto res = set_pos(pos.first, pos.second);
    if (res) {
        m_recordings.emplace_back(direction);
    }

    return res;
}

// private
bool CRobot::would_move_succeed(int x, int y) const {
    if (m_world.is_cell_out_of_bounds(x, y)) return false;
    if (m_world.is_cell_blocked(x, y)) return false;
    return true;
}

bool CRobot::set_pos( int x, int y ) {
    if (!would_move_succeed(x, y)) return false;

    m_pos_x = x;
    m_pos_y = y;
    m_visit_counter += 1;
    m_visited_cells[y * g_world_size + x] = m_visit_counter;
    return true;
}

std::pair<int, int> CRobot::get_adjacent_pos(Direction direction) const {
    int x{ m_pos_x }, y{ m_pos_y };

    switch (direction) {
    case Direction::UP: {
        y -= 1;
    } break;
    case Direction::DOWN: {
        y += 1;
    } break;
    case Direction::LEFT: {
        x -= 1;
    } break;
    case Direction::RIGHT: {
        x += 1;
    } break;
    }

    return { x, y };
}

std::optional<std::pair<int, int>> CRobot::get_target_location() const {
    for (int y{ -g_view_radius }; y <= g_view_radius; y++) {
        for (int x{ -g_view_radius }; x <= g_view_radius; x++) {
            auto wx = m_pos_x + x;
            auto wy = m_pos_y + y;

            if (m_world.get_cell_type(wx, wy) == ItemType::TARGET) {
                return std::pair{ wx, wy };
            }
        }
    }

    return std::nullopt;
}
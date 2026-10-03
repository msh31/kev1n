#include "robot.hpp"
#include "globals.hpp"

void CRobot::draw( Color color ) {
    DrawRectangle( m_pos_x * g_cell_pixel_size, m_pos_y * g_cell_pixel_size, g_cell_pixel_size, g_cell_pixel_size, color );
}

void CRobot::update() {
    if (m_kev.is_thinking()) return;

    if (auto decision = m_kev.try_get_decision()) {
        bool res = move(decision->direction);
        if (res) {
            if (m_world.get_cell_type(m_pos_x, m_pos_y) == ItemType::TARGET) {
                reached_destination = true;
            }
        }
    }
    m_kev.request_decision(build_state());
}

bool CRobot::move( Direction direction ) {
    auto c_pos = get_pos( );

    int x{ c_pos.first }, y{ c_pos.second };

    switch ( direction ) {
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

    return set_pos( x, y );
}

// private
bool CRobot::set_pos( int x, int y ) {
    if (m_world.is_cell_out_of_bounds(x, y)) return false;
    if ( m_world.is_cell_blocked( x, y ) ) return false;

    m_pos_x = x;
    m_pos_y = y;
    m_visited_cells[y * g_world_size + x] = true;
    return true;
}

std::string CRobot::build_state() {
    std::string str{};

    for (int y{-2}; y <= 2; y++) {
        for (int x{ -2 }; x <= 2; x++) {
            auto wx = m_pos_x + x;
            auto wy = m_pos_y + y;

            if (m_world.is_cell_out_of_bounds(wx, wy)) {
                str += '#';
                continue;
            }

            if (x == 0 && y == 0) {
                str += 'R'; //robot
                continue;
            }

            auto type = m_world.get_cell_type(wx, wy);
            bool visited = m_visited_cells[wy * g_world_size + wx];
            if (type == ItemType::NONE && visited) {
                str += 'V'; //visited
                continue;
            }

            str += legend[static_cast<size_t>(type)].symbol;
        }
        str += '\n';
    }

    return str;
}
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
#ifndef NDEBUG
        std::println("decision: {}, moved?: {}", reverse_lookup.at(decision->direction), res);
#endif
    }
    m_kev.request_decision(build_state(), allowed_directions());
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

std::vector<Direction> CRobot::allowed_directions() const {
    std::vector<Direction> unvisited{};
    std::vector<Direction> closer{};
    std::vector<Direction> closer_unvisited{};

    int distance_to_target = -1;

    auto target = get_target_location();
    if (target.has_value()) {
        distance_to_target = std::abs(target->first - m_pos_x) + std::abs(target->second - m_pos_y);
    }

    std::array<Direction, 4> directions = {
        Direction::UP,
        Direction::DOWN,
        Direction::LEFT,
        Direction::RIGHT
    };

    int lowest_time = INT_MAX;
    Direction timed_direction{};

    for (const auto& d : directions) {
        auto [x, y] = get_adjacent_pos(d);
        if (!would_move_succeed(x, y)) continue;

        auto vtime = m_visited_cells[y * g_world_size + x];
        if(vtime == 0) unvisited.push_back(d);

        if (vtime < lowest_time) {
            lowest_time = vtime;
            timed_direction = d;
        }

        //target search
        if (target.has_value()) {
            auto neighbor_distance = std::abs(target->first - x) + std::abs(target->second - y);
            if (neighbor_distance < distance_to_target) {
                closer.push_back(d);
                if (vtime == 0) {
                    closer_unvisited.push_back(d);
                }
            }
        }
    }

    if (lowest_time == INT_MAX) return {};
    if (!closer_unvisited.empty()) return closer_unvisited;
    if (!closer.empty()) return closer;
    if (!unvisited.empty()) return unvisited;
    return { timed_direction };
}

std::string CRobot::build_state() {
    std::string str{};

    for (int y{-g_view_radius }; y <= g_view_radius; y++) {
        for (int x{ -g_view_radius }; x <= g_view_radius; x++) {
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

#ifndef NDEBUG
    std::println("[DEBUG] state built:\n{}", str);
#endif
    return str;
}
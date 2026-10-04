#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;

enum class Direction { UP, DOWN, LEFT, RIGHT };

enum class ItemType { NONE, OBSTACLE, TARGET, STATION };
struct ItemSymbol {
	char symbol;
	std::string_view meaning;
};

static const std::unordered_map<std::string, Direction> lookup = {
    { "up", Direction::UP }, { "down", Direction::DOWN }, { "left", Direction::LEFT }, { "right", Direction::RIGHT } };
static const std::unordered_map<Direction, std::string> reverse_lookup = {
    { Direction::UP, "up" }, { Direction::DOWN, "down" }, { Direction::LEFT, "left" }, { Direction::RIGHT, "right" } };

#pragma once

enum class Direction { UP, DOWN, LEFT, RIGHT };

enum class ItemType { NONE, OBSTACLE, TARGET, STATION };
struct ItemSymbol {
	char symbol;
	std::string_view meaning;
};
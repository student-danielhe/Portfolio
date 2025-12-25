#pragma once
#include "Map.hpp"
/*Functionalities:
1. Find path from point x y in map z to point x' y' in map z'
2. Go through  stairs
3. Go through/ blocked by shortcuts
*/


enum class MovementCommands {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    INTERACT
};

std::vector<MovementCommands> GetPathTo(SpecificPoint from, SpecificPoint to);
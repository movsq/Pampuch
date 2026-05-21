#pragma once

#include "Level.h"
#include <string>
#include <vector>

struct MapPack {
    std::string name;
    std::string author;
    std::vector<Level> levels;
};

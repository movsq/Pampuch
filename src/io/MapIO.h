#pragma once

#include "world/MapPack.h"
#include <string>
#include <vector>

namespace MapIO {

    void EnsureFolder();

    bool SavePack(const MapPack& pack);
    [[nodiscard]] bool LoadPack(const std::string& filepath, MapPack& outPack);
    [[nodiscard]] std::vector<std::string> GetAvailablePacks();

}

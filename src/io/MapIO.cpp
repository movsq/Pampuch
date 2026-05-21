#include "MapIO.h"
#include "AssetPaths.h"
#include "raylib.h"
#include "../../deps/json.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace {

void to_json(json& j, const Level& l) {
    j = json::object();
    j["tiles"] = json::array();
    for (int x = 0; x < Config::Grid::COLS; ++x) {
        json col = json::array();
        for (int y = 0; y < Config::Grid::ROWS; ++y) {
            col.push_back(static_cast<int>(l.tiles[x][y]));
        }
        j["tiles"].push_back(col);
    }
    j["startX"] = l.startPos.x;
    j["startY"] = l.startPos.y;
    j["ghostSpawns"] = json::array();
    for (const auto& g : l.ghostSpawns) {
        j["ghostSpawns"].push_back(json::array({ g.x, g.y }));
    }
}

void from_json(const json& j, Level& l) {
    if (j.contains("tiles") && j["tiles"].is_array()) {
        for (int x = 0; x < Config::Grid::COLS && x < static_cast<int>(j["tiles"].size()); ++x) {
            const auto& col = j["tiles"][x];
            for (int y = 0; y < Config::Grid::ROWS && y < static_cast<int>(col.size()); ++y) {
                l.tiles[x][y] = static_cast<TileType>(col[y].get<int>());
            }
        }
    }
    l.startPos.x = j.value("startX", 0);
    l.startPos.y = j.value("startY", 0);
    l.ghostSpawns.clear();
    if (j.contains("ghostSpawns") && j["ghostSpawns"].is_array()) {
        for (const auto& g : j["ghostSpawns"]) {
            if (g.is_array() && g.size() >= 2) {
                l.ghostSpawns.push_back({ g[0].get<int>(), g[1].get<int>() });
            }
        }
    }
}

} // namespace

namespace MapIO {

void EnsureFolder() {
    if (!fs::exists(AssetPaths::MAP_FOLDER)) {
        fs::create_directory(AssetPaths::MAP_FOLDER);
    }
}

bool SavePack(const MapPack& pack) {
    EnsureFolder();
    const std::string filename = std::string(AssetPaths::MAP_FOLDER) + pack.name + AssetPaths::MAP_EXTENSION;
    std::ofstream out(filename);
    if (!out) {
        TraceLog(LOG_ERROR, "MAP_IO: Failed to open file for writing: %s", filename.c_str());
        return false;
    }

    try {
        json j;
        j["name"]   = pack.name;
        j["author"] = pack.author;
        j["levels"] = json::array();
        for (const auto& lvl : pack.levels) {
            json jl;
            to_json(jl, lvl);
            j["levels"].push_back(jl);
        }
        out << j.dump(4);
        TraceLog(LOG_INFO, "MAP_IO: Saved MapPack: %s", filename.c_str());
        return true;
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "MAP_IO: Failed to serialize MapPack: %s", e.what());
        return false;
    }
}

bool LoadPack(const std::string& filepath, MapPack& outPack) {
    std::ifstream in(filepath);
    if (!in) {
        TraceLog(LOG_ERROR, "MAP_IO: Failed to open file: %s", filepath.c_str());
        return false;
    }

    json j;
    try {
        in >> j;
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "MAP_IO: JSON parse error in %s: %s", filepath.c_str(), e.what());
        return false;
    }

    outPack.name   = j.value("name",   std::string());
    outPack.author = j.value("author", std::string());
    outPack.levels.clear();

    for (const auto& jl : j.value("levels", json::array())) {
        Level lvl;
        from_json(jl, lvl);
        outPack.levels.push_back(lvl);
    }

    TraceLog(LOG_INFO, "MAP_IO: Loaded MapPack: %s (%zu levels)",
        outPack.name.c_str(), outPack.levels.size());
    return true;
}

std::vector<std::string> GetAvailablePacks() {
    EnsureFolder();
    std::vector<std::string> files;
    try {
        for (const auto& entry : fs::directory_iterator(AssetPaths::MAP_FOLDER)) {
            if (entry.path().extension() == AssetPaths::MAP_EXTENSION) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        TraceLog(LOG_ERROR, "MAP_IO: Failed to enumerate map packs: %s", e.what());
    }
    return files;
}

} // namespace MapIO

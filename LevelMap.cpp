#include "LevelMap.hpp"
#include "load_save_png.hpp"

LevelMap::LevelMap(std::string map_name) {

    // Get tile from color
    auto getTile = [](const glm::u8vec4 &color) {

        const glm::u8vec4 Black = glm::u8vec4(0, 0, 0, 255);
        const glm::u8vec4 White = glm::u8vec4(255, 255, 255, 255);
        const glm::u8vec4 Blue = glm::u8vec4(0, 0, 255, 255);
        const glm::u8vec4 Red = glm::u8vec4(255, 0, 0, 255);
        const glm::u8vec4 Green = glm::u8vec4(0, 255, 0, 255);

        if (color == Black)      return Tile::Floor;
        else if (color == White) return Tile::Wall;
        else if (color == Blue)  return Tile::Spikes;
        else if (color == Red)   return Tile::EnemySpawner;
        else if (color == Green) return Tile::PlayerSpawner;
        else {
            throw std::runtime_error("Failed to find correct tile type, color found: %d, %d, %d, %d", color.x, color.y, color.z, color.w);
            return Tile::TILE_ERROR;
        }
    };

    // Get tile map from png
    std::vector<glm::u8vec4> map_raw_data;
    load_png(map_name, &size, &map_raw_data, LowerLeftOrigin);
    for (int row = 0; row < size.y; row++) {
        for (int col = 0; col < size.x; col++) {
            tiles[col + (size.x * row)] = getTile(map_raw_data[col + (size.x * row)]);
        }
    }
}
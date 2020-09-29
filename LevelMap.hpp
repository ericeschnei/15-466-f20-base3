#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

struct LevelMap {
	LevelMap(std::string map_name);
	
	enum Tile {
		Floor,
		Spikes,
		PlayerSpawner,
		EnemySpawner,
		Wall
	};

	glm::uvec2 size;
	std::vector<Tile> tiles;

	static inline bool is_collidable(Tile t) {
		return t == Wall;
	}


};

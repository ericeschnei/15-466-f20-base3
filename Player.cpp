#include "Player.hpp"

Player::~Player() {}

bool Player::move(Direction d, LevelMap &map) {
	// check if we're already moving
	if (euler_angles.is_tweening() || position.is_tweening()) {
		return false;
	}

	// get new position
	glm::vec3 current_position = position.get();
	glm::ivec2 new_pos = glm::ivec2(
			(int)glm::round(current_position.x), 
			(int)glm::round(current_position.y)
	);

	switch (d) {
		case Player::Direction::Up: 
			new_pos += glm::ivec2(0, 1);
			break;
		case Player::Direction::Down:
			new_pos += glm::ivec2(0, -1);
			break;
		case Player::Direction::Left:
			new_pos += glm::ivec2(-1, 0);
			break;
		case Player::Direction::Right:
			new_pos += glm::ivec2(1, 0);
			break;
	}
	
	// check if out of map range
	if (new_pos.x < 0 || new_pos.x >= map.size.x) {
		return false;
	}
	if (new_pos.y < 0 || new_pos.y >= map.size.y) {
		return false;
	}

	// check if tile is collidable
	if (LevelMap::is_collidable(map.tiles[new_pos.x + map.size.x * new_pos.y])) {
		return false;
	}

	// actually move
	position.queue_motion(glm::vec3(glm::vec2(new_pos), 0.0f), Player::MoveTime);
	return true;

}

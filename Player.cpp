#include "Player.hpp"

Player::~Player() {}

void Player::update(float elapsed) {
	position.get(elapsed);
	euler_angles.get(elapsed);
}

bool Player::move(Direction d, LevelMap &map) {
	// check if we're already moving
	if (euler_angles.is_tweening() || position.is_tweening()) {
		return false;
	}

	// get new position
	glm::ivec2 new_pos = get_new_pos(d);
	
	// check if out of map range
	if (new_pos.x < 0 || new_pos.x >= map.size.x * LevelMap::TileSize) {
		return false;
	}
	if (new_pos.y < 0 || new_pos.y >= map.size.y * LevelMap::TileSize) {
		return false;
	}

	// check if tile is collidable
	if (LevelMap::is_collidable(map.tiles[(new_pos.x + (map.size.x * new_pos.y)) / (size_t)LevelMap::TileSize])) {
		return false;
	}

	// actually move
	position.queue_motion(glm::vec3(glm::vec2(new_pos), 0.0f), Player::MoveTime);
	return true;

}

bool Player::attack(Direction d, std::vector<Enemy>& enemies)
{
	if (position.is_tweening()) return false;

	glm::ivec2 new_pos = get_new_pos(d);
	// TODO implement enemy
	for (Enemy enemy : enemies) {
		if (enemy.get_rounded_pos() == new_pos && enemy.is_alive()) {
			enemy.die();
			return true;
		}
	}
	return false;
}

glm::ivec2 Player::get_new_pos(Direction d)
{
	glm::ivec2 new_pos = get_rounded_pos();

	switch (d) {
		case Player::Direction::Up:
			new_pos += glm::ivec2(0, 1 * LevelMap::TileSize);
			break;
		case Player::Direction::Down:
			new_pos += glm::ivec2(0, -1 * LevelMap::TileSize);
			break;
		case Player::Direction::Left:
			new_pos += glm::ivec2(-1 * LevelMap::TileSize, 0);
			break;
		case Player::Direction::Right:
			new_pos += glm::ivec2(1 * LevelMap::TileSize, 0);
			break;
	}
	return new_pos;
}

glm::ivec2 Player::get_rounded_pos() {
	glm::vec3 current_position = position.get();
	glm::ivec2 new_pos = glm::ivec2(
		(int)glm::round(current_position.x),
		(int)glm::round(current_position.y)
	);
	return new_pos;
}

#include "Enemy.hpp"

glm::ivec2 Enemy::get_rounded_pos() {return glm::ivec2();}
Enemy::~Enemy() {};

void Enemy::spawn_enemy()
{
	alive = true;
}

void Enemy::die()
{
	alive = false;
}

bool Enemy::is_alive()
{
	return alive;
}

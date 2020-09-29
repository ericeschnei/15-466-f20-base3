#include "Mode.hpp"

#include "LevelMap.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#include "LevelMap.hpp"
#include "Player.hpp"

struct PlayMode : Mode {
	PlayMode(const std::string &map_path);
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene blender_scene;
	Scene scene;

	LevelMap map;
	
	static constexpr size_t NumEnemies = 70;
	std::array< Scene::Drawable * , NumEnemies > enemy_drawables;
	std::array< Tweener<glm::vec3>, NumEnemies > enemies;
	float timer = 0.0f;

	std::vector< Scene::Drawable * > floor_drawables;
	std::vector< Scene::Drawable * > spike_drawables;
	glm::vec3 camera_offset = glm::vec3(0.0f, 0.0f, 200.0f);
	glm::quat camera_direction = glm::quat(glm::vec3(0.0f,0.0f,0.0f));
	Scene::Drawable *player_drawable;

	Player *player;

	// vector of all enemies alive and dead

	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

	glm::vec2 get_random_pos(glm::uvec2 bounds);
};

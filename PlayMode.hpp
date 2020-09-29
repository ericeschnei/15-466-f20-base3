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
	std::vector< Scene::Drawable * > floor_drawables;
	std::vector< Scene::Drawable * > spike_drawables;
	std::vector< Scene::Drawable * > enemy_drawables;
	std::vector< Scene::Drawable * > arrow_drawables;
	Scene::Drawable *player_drawable;
	Scene::Drawable *bow_drawable;
	Scene::Drawable *sword_drawable;

	Player *player;
	//music coming from the tip of the leg (as a demonstration):
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};

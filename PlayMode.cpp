#include "PlayMode.hpp"

#include "LevelMap.hpp"
#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <ostream>
#include <random>
#include <ctime>

GLuint meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("game3.pnct"));
	meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > main_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("game3.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

//Load< Sound::Sample > dusty_floor_sample(LoadTagDefault, []() -> Sound::Sample const * {
//	return new Sound::Sample(data_path("dusty-floor.opus"));
//});

Load< Sound::Sample > level_one_sample(LoadTagDefault, []() -> Sound::Sample const* {
	return new Sound::Sample(data_path("game3bg.wav"));
});

glm::vec2 PlayMode::get_random_pos(glm::uvec2 bounds) {
	static std::mt19937 mt = std::mt19937((unsigned int)time(0));

	auto r = glm::vec2((float)(mt() % bounds.x), (float)(mt() % bounds.y));	
	return glm::clamp(4.0f * r - glm::vec2(bounds), glm::vec2(0.0f, 0.0f), 2.0f * glm::vec2(bounds) - glm::vec2(2.0f, 2.0f));

}

PlayMode::PlayMode(const std::string &map_path) : blender_scene(*main_scene), map(LevelMap(data_path(map_path))) {
	//get pointer to camera for convenience:
	if (blender_scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(blender_scene.cameras.size()));
	auto old_camera = blender_scene.cameras.front();

	scene.cameras.push_back(old_camera);
	camera = &scene.cameras.front();

	auto copy_drawable = [](Scene::Drawable *drawable) -> Scene::Drawable * {
		Scene::Drawable *ret = new Scene::Drawable( new Scene::Transform() );
		ret->transform->name = drawable->transform->name;
		ret->transform->position = drawable->transform->position;
		ret->transform->rotation = drawable->transform->rotation;
		ret->transform->scale = drawable->transform->scale;
		ret->transform->parent = drawable->transform->parent;

		ret->pipeline = drawable->pipeline;
		return ret;
	};

	Scene::Drawable *old_floor = nullptr;
	Scene::Drawable *old_spike = nullptr;
	for (Scene::Drawable &d : blender_scene.drawables) {
		std::cout << d.transform->name << std::endl;
		if (d.transform->name == "Player") {
			player_drawable = copy_drawable(&d);
		} else if (d.transform->name == "Enemy") {
			for (size_t i = 0; i < NumEnemies; i++) {
				Scene::Drawable *copy = copy_drawable(&d);
				enemy_drawables[i] = copy;
				scene.drawables.push_back(*copy);
				enemies[i].override_motion(glm::vec3(get_random_pos(map.size), 0.0f));
			}
		} else if (d.transform->name == "Hole") {
			old_spike = &d;
		} else if (d.transform->name == "Floor") {
			old_floor = &d;
		}
	}

	for (size_t y = 0; y < map.size.y; y++) {
		for (size_t x = 0; x < map.size.x; x++) {
			LevelMap::Tile t = map.tiles[y * map.size.x + x];
			glm::vec3 pos = LevelMap::TileSize * glm::vec3((float)x, (float)y, 0.0f);

			// create floor
			Scene::Drawable *d;
			d = copy_drawable(old_floor);
			scene.drawables.push_back(*d);
			d->transform->position = pos;

			if (t == LevelMap::Tile::Spikes) {
				Scene::Drawable *d;
				d = copy_drawable(old_spike);
				scene.drawables.push_back(*d);
				spike_drawables.push_back(d);
				d->transform->position = pos;
			}

			if (t == LevelMap::Tile::PlayerSpawner) {
				scene.drawables.push_back(*player_drawable);
				player_drawable->transform->position = pos;
				player = new Player(pos);

				camera->transform->position = pos + camera_offset;
				camera->transform->rotation = camera_direction;
			}
		}
	}

	Sound::loop(*level_one_sample);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	// update player
	player->update(elapsed);
	timer += elapsed;
	if (timer > 0.5f) timer -= 0.5f;
	std::cout << timer << "\n";
	
	punishment = std::max(punishment - elapsed, 0.0f);
	player_drawable->transform->position = player->position.get();
	
	if ((timer < radius || 0.5f - timer < radius)) {
		if (punishment > 0.0f) {}
		else if (left.downs > 0) {
			player->move(Player::Left, map);
		} else if (right.downs > 0) {
			player->move(Player::Right, map);
		} else if (up.downs > 0) {
			player->move(Player::Up, map);
		} else if (down.downs > 0) {
			player->move(Player::Down, map);
		}
	}
	else if (left.pressed || right.pressed || up.pressed || down.pressed) {
		punishment = 0.5f;
		//TODO play punishment sound
	}
	
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;

	camera->transform->position = player->position.get() + camera_offset;
	//reset button press counters:
	
	
	for (size_t i = 0; i < NumEnemies; i++) {
		if (!enemies[i].is_tweening()) {
			enemies[i].queue_motion(glm::vec3(get_random_pos(map.size), 0.0f), 8.0f);
		}
		enemy_drawables[i]->transform->position = enemies[i].get(elapsed);

	}
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		if (timer < radius || 0.5f - timer < radius) {
			constexpr float H = 0.9f;
			lines.draw_text("NOW",
				glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text("NOW",
				glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + +0.1f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		}
		if (punishment > 0.0f) {
			std::cout << "BAD\n";
			constexpr float H = 0.9f;
			lines.draw_text("BAD",
				glm::vec3(aspect - 1.4f * H, -1.0 + 0.1f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text("BAD",
				glm::vec3(aspect - 1.4f * H + ofs, -1.0 + +0.1f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff, 0x00, 0x00, 0x00));
		}
	}
	GL_ERRORS();
}


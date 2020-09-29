#include "Scene.hpp"
#include "glm/fwd.hpp"
#include "Tweener.hpp"

struct Player {
	
	Player(Scene::Transform *_transform, glm::vec3 initial_position) : 
		transform(_transform), position(initial_position) {}
	~Player();
	
	Scene::Transform *transform;
	Tweener<glm::vec3> euler_angles = Tweener<glm::vec3>(glm::vec3());
	Tweener<glm::vec3> position;
	enum Direction {
		Up,
		Down,
		Left,
		Right
	};

	bool move(Direction d);
	bool attack(Direction d);
	bool draw_bow(Direction d);
	bool fire();
	bool fail();
	

};

#include "Scene.hpp"
#include "glm/fwd.hpp"
#include "Tweener.hpp"
#include "LevelMap.hpp"
#include "Enemy.hpp"
#include "Arrow.hpp"
#include <functional>
#include <vector>

struct Player {
	
	static constexpr float MoveTime = 0.25f;

	Player(glm::vec3 initial_position) : position(initial_position) {}
	~Player();


	Scene::Transform *transform;
	Tweener<glm::vec3> euler_angles = Tweener<glm::vec3>(glm::vec3());
	Tweener<glm::vec3> position;
	Tweener<int> invincible = Tweener<int>(0);

	enum Direction {
		Up,
		Down,
		Left,
		Right
	};

	void update(float elapsed);

	/* Attempt a player move. Returns true if move succeeded. */
	bool move(Direction d, LevelMap &map);

	/* Attempt an attack. Returns true if an enemy was killed. */
	bool attack(Direction d, std::vector<Enemy> &enemies);

	/* Draw a bow. */
	void draw_bow(Direction d);

	/* Fire the bow. arrow_drawable is a pointer to the drawable object to
	 * be used when emplacing a new arrow in arrows.
	 */
	void fire(Scene::Drawable *arrow_drawable, std::vector<Arrow> &arrows);
	
	/* Call when something did not succeed because it was done out of sync
	 * with the beat. Cancels bow firing.
	 * TODO stretch goal -- make the character visually communicate when they fail
	 */
	void fail();
	
	/* Call when the player takes damage. 
	 * invincibility - time whithin which the player is invincible.
	 */
	void damage(float invincibility);

	glm::ivec2 get_new_pos(Direction d);

	glm::ivec2 get_rounded_pos();

};

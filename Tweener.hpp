#pragma once
#include <deque>
#include "glm/glm.hpp"

template <typename T>
struct Tweener {

	T value;
	float period;
	float elapsed;
	std::deque<std::pair<float, T>> actions;

	/* Constructs a Tweener object.
	 * _start - the starting position of the object to be tweened
	 */
	Tweener(T _start) : value(_start) {}
	
	/* Returns whether there is currently an operation being performed.
	 * Useful to determine collision.
	 */
	bool is_tweening() {
		return (actions.size() > 0);
	};

	/* 
	 * Get the current tweened position of the T.
	 * if _elapsed is passed in, this will update *before* returning.
	 */
	T get(float _elapsed = 0.0f) {
		
		elapsed += _elapsed;
		while (actions.size() > 0 && elapsed >= actions.back().first) {
			value = actions.back().second;
			elapsed -= actions.back().first;
			actions.pop_back();
		}

		if (actions.size() == 0) {
			elapsed = 0.0f;
			return value;
		}

		float progress = elapsed / actions.back().first;

		// TODO: if we have time, let the tweening function be arbitrary
		return progress * actions.back().second + (1.0f - progress) * value;

	};

	/* Add a new animation to the queue.
	 * _end - the end position of the motion.
	 * _period - the time it takes to perform said motion.
	 */
	void queue_motion(T _end, float _period) {
		actions.emplace_front(_period, _end);
	};

	/* Destroy the current queue and force-update the
	 * current position of the value to _end.
	 */
	void override_motion(T _end) {
		actions.clear();
		value = _end;
	};

};

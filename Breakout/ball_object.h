#ifndef BALLOBJECT_H
#define BALLOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "texture.h"

class BallObject : public GameObject
{
public:
	// constructor
	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	// ball state
	float Radius;
	bool Stuck;
	bool Sticky, PassThrough;

	// move the ball, keeping it constrained within the windows bounds  => return new position
	glm::vec2 Move(float deltaTime, unsigned int window_width);

	// reset the ball to original state with given position and velocity
	void Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif

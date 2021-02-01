#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "shader.h"
#include "texture.h"
#include "game_object.h"

// represent a single particle and its state
struct Particle
{
	Particle() 
		: Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f)
	{  }

	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float Life;
};

class ParticleGenerator
{
public:
	// constructor
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);

	// update all particles
	void Update(float deltaTime, GameObject& object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

	// render all particles
	void Draw();

private:
	// state
	std::vector<Particle> particles;
	unsigned int amount;

	// render state
	Shader shader;
	Texture2D texture;
	unsigned int VAO;

	// initialize buffer and vertex attributes
	void Init();

	// return the first Particle index that is currently unused
	unsigned int lastUsedParticle = 0;
	unsigned int FirstUnusedParticle();

	// respawn particle
	void RespawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif


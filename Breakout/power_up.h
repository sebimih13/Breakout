#ifndef POWER_UP_H
#define POWER_UP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <irrKlang/irrKlang.h>

#include <string>
#include <vector>

#include "game_object.h"
#include "game_object.h"
#include "ball_object.h"
#include "post_processor.h"
#include "sprite_renderer.h"

// size of the PowerUp block
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);

// velocity a PowerUp block has when spawned
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
	// constructor
	PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
		: GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated(false)
	{  }

	// powerup state
	std::string Type;
	float Duration;
	bool Activated;
};

class PowerUpsManager
{
public:
	// constructor
	PowerUpsManager() {  };

	void SpawnPowerUps(GameObject& block);
	void UpdatePowerUps(float deltaTime, GameObject* Player, BallObject* Ball, PostProcessor* Effects);
	void DrawPowerUps(SpriteRenderer* Renderer);
	void CheckCollision(GameObject* Player, BallObject* Ball, PostProcessor* Effects, irrklang::ISoundEngine* SoundEngine, unsigned int Height, unsigned int Width);
	void Reset();

private:
	// state
	std::vector<PowerUp> PowerUps;

	// utility
	bool ShouldSpawn(int chance);
	void ActivatePowerUp(PowerUp& powerup, GameObject* Player, BallObject* Ball, PostProcessor* Effects, unsigned int width);
	bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);

	bool CheckCollisionObjects(GameObject& one, GameObject& two);
};

#endif

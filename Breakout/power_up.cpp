#include "power_up.h"
#include "resource_manager.h"

#include <iostream>

bool PowerUpsManager::ShouldSpawn(unsigned int chance)
{
	return rand() % 100 < chance;
}

void PowerUpsManager::SpawnPowerUps(GameObject& block)
{
	if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_speed")));
	else if (ShouldSpawn(25))
		this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 2.0f), 3.0f, block.Position, ResourceManager::GetTexture("powerup_sticky")));
	else if (ShouldSpawn(20))
		this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 5.0f, block.Position, ResourceManager::GetTexture("powerup_passthrough")));
	else if (ShouldSpawn(20))
		this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f, block.Position, ResourceManager::GetTexture("powerup_increase")));
	else if (ShouldSpawn(15))
		this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 7.0f, block.Position, ResourceManager::GetTexture("powerup_chaos")));
	else if (ShouldSpawn(10))
		this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 6.0f, block.Position, ResourceManager::GetTexture("powerup_confuse")));
}

void PowerUpsManager::UpdatePowerUps(float deltaTime, GameObject* Player, BallObject* Ball, PostProcessor* Effects)
{
	for (PowerUp& powerUp : this->PowerUps)
	{
		powerUp.Position += powerUp.Velocity * deltaTime;

		if (powerUp.Activated)
		{
			powerUp.Duration -= deltaTime;

			if (powerUp.Duration <= 0.0f)
			{
				// remove from list
				powerUp.Activated = false;

				// deactivate effects
				if (powerUp.Type == "sticky" && !IsOtherPowerUpActive(this->PowerUps, "sticky"))
				{
					Ball->Sticky = false;
					Player->Color = glm::vec3(1.0f);
				}
				else if (powerUp.Type == "pass-through" && !IsOtherPowerUpActive(this->PowerUps, "pass-through"))
				{
					Ball->PassThrough = false;
					Ball->Color = glm::vec3(1.0f);
				}
				else if (powerUp.Type == "confuse" && !IsOtherPowerUpActive(this->PowerUps, "confuse"))
				{
					Effects->Confuse = false;
				}
				else if (powerUp.Type == "chaos" && !IsOtherPowerUpActive(this->PowerUps, "chaos"))
				{
					Effects->Chaos = false;
				}
			}
		}
	}

	this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
		[](const PowerUp& poweruUp) { return poweruUp.Destroyed && !poweruUp.Activated; }), this->PowerUps.end());
}

void PowerUpsManager::Reset()
{
	for (PowerUp& powerup : PowerUps)
	{
		powerup.Duration = 0.0f;
		powerup.Destroyed = true;
	}
}

void PowerUpsManager::ActivatePowerUp(PowerUp& powerup, GameObject* Player, BallObject* Ball, PostProcessor* Effects, unsigned int width)
{
	if (powerup.Type == "speed")
	{
		Ball->Velocity *= 1.2f;
	}
	else if (powerup.Type == "sticky")
	{
		Ball->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerup.Type == "pass-through")
	{
		Ball->PassThrough = true;
		Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerup.Type == "pad-size-increase")
	{
		if (Player->Size.x < width)
			Player->Size.x += 100;
		if (Player->Size.x >= width)
			Player->Size.x = width;
	}
	else if (powerup.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = true;
	}
	else if (powerup.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

bool PowerUpsManager::IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
	for (const PowerUp& powerUp : powerUps)
		if (powerUp.Activated && powerUp.Type == type)
			return true;
	return false;
}

void PowerUpsManager::DrawPowerUps(SpriteRenderer* Renderer)
{
	for (PowerUp& powerup : this->PowerUps)
		if (!powerup.Destroyed)
			powerup.Draw(*Renderer);
}

void PowerUpsManager::CheckCollision(GameObject* Player, BallObject* Ball, PostProcessor* Effects, unsigned int Height, unsigned int Width)
{
	for (PowerUp& powerup : this->PowerUps)
	{
		if (!powerup.Destroyed)
		{
			// check if powerup passed bottom edge
			if (powerup.Position.y >= Height)
				powerup.Destroyed = true;

			if (CheckCollisionObjects(*Player, powerup))
			{
				ActivatePowerUp(powerup, Player, Ball, Effects, Width);
				powerup.Destroyed = true;
				powerup.Activated = true;
			}
		}
	}
}

bool PowerUpsManager::CheckCollisionObjects(GameObject& one, GameObject& two)
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;

	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

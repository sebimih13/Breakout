#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "power_up.h"

#include <string>

// components
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;
PowerUpsManager* PowerUpManager;

Game::Game(unsigned int width, unsigned int height)
	: State(GameState::GAME_ACTIVE), Keys(), Width(width), Height(height)
{  
	ShakeTime = 0.0f;
}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particles;
	delete Effects;
	delete PowerUpManager;
}

void Game::Init()
{
	// load shaders
	ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("shaders/particle.vert", "shaders/particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("shaders/post_processing.vert", "shaders/post_processing.frag", nullptr, "postprocessing");

	// configure shader
	glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);
	ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);

	// load textures
	ResourceManager::LoadTexture("assets/background.jpg", false, "background");
	ResourceManager::LoadTexture("assets/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("assets/block.png", false, "block");
	ResourceManager::LoadTexture("assets/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("assets/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("assets/particle.png", true, "particle");

	ResourceManager::LoadTexture("assets/powerup_speed.png", true, "powerup_speed");
	ResourceManager::LoadTexture("assets/powerup_chaos.png", true, "powerup_chaos");
	ResourceManager::LoadTexture("assets/powerup_confuse.png", true, "powerup_confuse");
	ResourceManager::LoadTexture("assets/powerup_increase.png", true, "powerup_increase");
	ResourceManager::LoadTexture("assets/powerup_passthrough.png", true, "powerup_passthrough");
	ResourceManager::LoadTexture("assets/powerup_sticky.png", true, "powerup_sticky");

	// load levels
	for (int i = 1; i <= 5; i++)
	{
		GameLevel lvl;
		lvl.Load(("levels/level_" + std::to_string(i) + ".lvl").c_str(), Width, Height / 2);
		this->Levels.push_back(lvl);
	}
	this->Level = 0;

	// configure player
	glm::vec2 PlayerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(PlayerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	// configure ball
	glm::vec2 BallPos = PlayerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(BallPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

	// configure renderer
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// congfiure particle system
	Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);

	// congfiure effects system
	Effects = new PostProcessor(ResourceManager::GetShader("postprocessing"), this->Width, this->Height);

	// configure powerups manager
	PowerUpManager = new PowerUpsManager();
}

void Game::ProcessInput(float deltaTime)
{
	if (State == GAME_ACTIVE)
	{
		float velocity = PLAYER_VELOCITY * deltaTime;

		if (Keys[GLFW_KEY_SPACE])
			Ball->Stuck = false;

		if (Keys[GLFW_KEY_A] && Player->Position.x >= 0.0f)
		{
			Player->Position.x -= velocity;
			if (Ball->Stuck)
				Ball->Position.x -= velocity;
		}
		if (Keys[GLFW_KEY_D] && Player->Position.x <= Width - Player->Size.x)
		{
			Player->Position.x += velocity;
			if (Ball->Stuck)
				Ball->Position.x += velocity;
		}
	}
}

void Game::Update(float deltaTime)
{
	// update object
	Ball->Move(deltaTime, Width);

	// check for collision
	DoCollisions();

	// update particles
	Particles->Update(deltaTime, *Ball, 3, glm::vec2(Ball->Radius / 2.0f));

	// update PowerUps
	PowerUpManager->UpdatePowerUps(deltaTime, Player, Ball, Effects);

	// reduce shake time
	if (ShakeTime > 0.0f)
	{
		ShakeTime -= deltaTime;
		if (ShakeTime <= 0.0f)
			Effects->Shake = false;
	}

	// check loss condition
	if (Ball->Position.y >= Height)
	{
		ResetLevel();
		ResetPlayer();
		PowerUpManager->Reset();
	}
}

void Game::Render()
{
	if (State == GAME_ACTIVE)
	{
		// post-processing effects
		Effects->BeginRender();

			// draw background
			Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f);

			// draw level
			Levels[Level].Draw(*Renderer);

			// draw player
			Player->Draw(*Renderer);

			// draw PowerUps
			PowerUpManager->DrawPowerUps(Renderer);

			// draw particles
			Particles->Draw();

			// draw ball
			Ball->Draw(*Renderer);

		Effects->EndRender();
		Effects->Render((float)glfwGetTime());
	}
}

void Game::DoCollisions()
{
	for (GameObject& box : this->Levels[this->Level].Bricks)
	{
		if (!box.Destroyed)
		{
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision))	// if collision is true
			{
				// destroy block if not solid
				if (!box.IsSolid)
				{
					box.Destroyed = true;
					PowerUpManager->SpawnPowerUps(box);
				}
				else
				{
					ShakeTime = 0.05f;
					Effects->Shake = true;
				}

				// collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);

				if (!(Ball->PassThrough && !box.IsSolid))
				{
					if (dir == LEFT || dir == RIGHT)	// horizontal collision
					{
						Ball->Velocity.x = -Ball->Velocity.x;

						// relocate
						float penetration = Ball->Radius - std::abs(diff_vector.x);
						if (dir == LEFT)
							Ball->Position.x += penetration;
						else
							Ball->Position.y -= penetration;
					}
					else	// vertical collision
					{
						Ball->Velocity.y = -Ball->Velocity.y;

						// relocate
						float penetration = Ball->Radius - std::abs(diff_vector.y);
						if (dir == UP)
							Ball->Position.y -= penetration;
						else
							Ball->Position.y += penetration;
					}
				}
			}
		}
	}

	// check collision for player pad and powerups
	PowerUpManager->CheckCollision(Player, Ball, Effects, this->Height, this->Width);

	// check collisions for player pad
	Collision result = CheckCollision(*Ball, *Player);
	if (!Ball->Stuck && std::get<0>(result))
	{
		// check where it hit the board and change the velocity based on where it hit the board
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);

		float strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		Ball->Velocity.y = -Ball->Velocity.y;

		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);

		// fix sticky paddle
		Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

		// if sticky powerup is active => stick ball to padle
		Ball->Stuck = Ball->Sticky;
	}
}

bool Game::CheckCollision(GameObject& one, GameObject& two)
{
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
					  two.Position.x + two.Size.x >= one.Position.x;

	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
					  two.Position.y + two.Size.y >= one.Position.y;

	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject& one, GameObject& two)
{
	// get center point circle
	glm::vec2 center = one.Position + one.Radius;

	// get center square
	glm::vec2 aabb_half_extents = glm::vec2(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center = glm::vec2(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);

	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);

	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;

	if (glm::length(difference) < one.Radius)
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Direction Game::VectorDirection(glm::vec2 target)
{
	glm::vec2 compass[] = {
		glm::vec2( 0.0f,  1.0f),	// up
		glm::vec2( 1.0f,  0.0f),	// right
		glm::vec2( 0.0f, -1.0f),	// down
		glm::vec2(-1.0f,  1.0f)	// left
	};

	float max = 0.0f;
	unsigned int best_match = -1;
	for (unsigned int i = 0; i < 4; i++)
	{
		float dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max)
		{
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}

void Game::ResetLevel()
{
	this->Levels[this->Level].Load(("levels/level_" + std::to_string(this->Level + 1) + ".lvl").c_str(), this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f), INITIAL_BALL_VELOCITY);
}


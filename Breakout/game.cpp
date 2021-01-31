#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "ball_object.h"

#include <string>

// components
SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;

Game::Game(unsigned int width, unsigned int height)
	: State(GameState::GAME_ACTIVE), Keys(), Width(width), Height(height)
{  }

Game::~Game()
{
	delete Renderer;
	delete Player;
}

void Game::Init()
{
	// load shaders
	ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", nullptr, "sprite");

	// configure shader
	glm::mat4 projection = glm::ortho(0.0f, (float)this->Width, (float)this->Height, 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
	ResourceManager::GetShader("sprite").Use().SetMatrix4("projection", projection);

	// set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	// load textures
	ResourceManager::LoadTexture("assets/background.jpg", false, "background");
	ResourceManager::LoadTexture("assets/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("assets/block.png", false, "block");
	ResourceManager::LoadTexture("assets/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("assets/paddle.png", true, "paddle");

	// load levels
	for (int i = 1; i <= 5; i++)
	{
		GameLevel lvl;
		lvl.Load(("levels/level_" + std::to_string(i) + ".lvl").c_str(), Width, Height / 2);
		this->Levels.push_back(lvl);
	}
	Level = 0;

	// configure player
	glm::vec2 PlayerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(PlayerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	// configure ball
	glm::vec2 BallPos = PlayerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(BallPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
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
	Ball->Move(deltaTime, Width);
}

void Game::Render()
{
	if (State == GAME_ACTIVE)
	{
		// draw background
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f);

		// draw level
		Levels[Level].Draw(*Renderer);

		// draw player
		Player->Draw(*Renderer);

		// draw ball
		Ball->Draw(*Renderer);
	}
}


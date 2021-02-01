#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tuple>

#include "game_level.h"
#include "ball_object.h"

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction
{
	UP,
	RIGHT,
	DOWN,
	LEFT
};

// collision data
typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);

// Initial velocity of the player paddle
const float PLAYER_VELOCITY = 500.0f;

// Initial velocity of the ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);

// Radius of the ball object
const float BALL_RADIUS = 12.5f;

class Game
{
public:
	// constructor / destructor
	Game(unsigned int width, unsigned int height);
	~Game();

	// game state
	GameState State;
	bool Keys[1024];
	unsigned int Width, Height;
	std::vector<GameLevel> Levels;
	unsigned int Level;

	// initialize game state (load all shaders / textures / levels)
	void Init();

	// game loop
	void ProcessInput(float deltaTime);
	void Update(float deltaTime);
	void Render();
	void DoCollisions();

	// Collision
	Collision CheckCollision(BallObject& one, GameObject& two);
	Direction VectorDirection(glm::vec2 target);
	float ShakeTime;

	// Reset
	void ResetLevel();
	void ResetPlayer();
};

#endif

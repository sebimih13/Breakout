#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);

// Initial velocity of the player paddle
const float PLAYER_VELOCITY = 500.0f;

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
};

#endif

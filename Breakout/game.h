#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Represents the current state of the game
enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

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

	// initialize game state (load all shaders / textures / levels)
	void Init();

	// game loop
	void ProcessInput(float deltaTime);
	void Update(float deltaTime);
	void Render();
};

#endif

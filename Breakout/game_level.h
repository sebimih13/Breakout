#ifndef GAMELEVEL_H
#define GAMELEVEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

#include <vector>

class GameLevel
{
public:
	// constructor
	GameLevel() {  };

	// level state
	std::vector<GameObject> Bricks;

	// load level from file
	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

	// render level
	void Draw(SpriteRenderer& renderer);

	// check if the level is completed (all non-solid tiles are destroyed)
	bool IsCompleted();

private:
	// initialize level from tile data
	void Init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
};

#endif

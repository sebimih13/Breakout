#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"

class SpriteRenderer
{
public:
	// constructor / destructor
	SpriteRenderer(Shader& shader);
	~SpriteRenderer();

	void DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));

private:
	// Render state
	Shader shader;
	unsigned int quadVAO;

	// Initialize and configure the quad's buffer and vertex attributes
	void InitRenderData();
};

#endif


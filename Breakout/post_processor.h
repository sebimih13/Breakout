#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"

class PostProcessor
{
public:
	// constructor
	PostProcessor(Shader shader, unsigned int width, unsigned int height);

	// state
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width, Height;

	// options
	bool Confuse, Chaos, Shake;

	// prepare the postprocessor's framebuffer operations before rendering the game
	void BeginRender();

	// call after rendering the game, so it stores all rendered data into a texture object
	void EndRender();

	// render the PostProcessor texture quad
	void Render(float time);

private:
	// render state
	unsigned int MSFBO, FBO;	// MSFBO = multisampled FBO || FBO = regular framebuffer
	unsigned int RBO;			// used for multisampled color buffer
	unsigned int VAO;

	void InitRenderData();
};

#endif

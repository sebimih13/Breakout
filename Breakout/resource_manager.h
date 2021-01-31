#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <glad/glad.h>

#include <map>
#include <string>

#include "texture.h"
#include "shader.h"

class ResourceManager
{
public:
	// resource storage
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;

	// load and generate a shader program from files: vertex, fragment, geometry
	static Shader LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);

	// retrieve a stored shader
	static Shader& GetShader(std::string name);

	// load and generate a texture from file
	static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

	// retrieve a stored texture
	static Texture2D& GetTexture(std::string name);

	// de-allocate all loaded resources
	static void Clear();

private:
	ResourceManager() {  }

	// load and generate a shader from file
	static Shader LoadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

	// load a single texture from file
	static Texture2D LoadTextureFromFile(const char* file, bool alpha);
};

#endif

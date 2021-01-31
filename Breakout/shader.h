#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Shader
{
public:
	// constructor
	Shader() {  }

	// state
	unsigned int ID;

	// sets the current shader as active
	Shader& Use();

	// compile the shader from given source code
	void Compile(const char* vertexSource, const char* fragmentSource, const char* geometryShader = nullptr);

	// utility functions
	void SetFloat(const char* name, float value, bool useShader = false);
	void SetInteger(const char* name, int value, bool useShader = false);
	void SetVector2f(const char* name, const glm::vec2 &value, bool useShader = false);
	void SetVector3f(const char* name, const glm::vec3 &value, bool useShader = false);
	void SetVector4f(const char* name, const glm::vec4 &value, bool useShader = false);
	void SetMatrix4(const char* name, const glm::mat4 &value, bool useShader = false);

private:
	// checks if compilation or linking failed and print errors
	void CheckCompileErrors(unsigned int object, std::string type);
};

#endif

#include "Rendering/Shader.h"

#include <glad/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// Retrieve the vertex/fragment source code from filePath
	std::string vertexCode, fragmentCode;
	std::ifstream vertexShaderFile, fragmentShaderFile;

	// Ensure file stream objects can throw exceptions:
	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open files
		vertexShaderFile.open(vertexPath);
		fragmentShaderFile.open(fragmentPath);
		std::stringstream vertexShaderStream, fragmentShaderStream;
		// Read file's buffer contents into streams
		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();
		// Close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vertexShaderStream.str();
		fragmentCode = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vertexShaderCode = vertexCode.c_str();
	const char* fragmentShaderCode = fragmentCode.c_str();

	// Compile shaders
	uint32_t vertex, fragment;
	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");
	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");


	// Shader Program
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glLinkProgram(m_ID);
	CheckCompileErrors(m_ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);


	// Get uniform locations
	int32_t count;

	int32_t size;
	GLenum type;

	const size_t bufSize = 32;
	GLchar name[bufSize];
	int32_t length;

	glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &count);

	for (size_t i = 0; i < count; i++)
	{
		glGetActiveUniform(m_ID, i, bufSize, &length, &size, &type, name);
		uint32_t location = glGetUniformLocation(m_ID, name);
		m_UniformLookup[name] = location;
	}
}

void Shader::Use()
{
	glUseProgram(m_ID);
}

void Shader::CheckCompileErrors(uint32_t shader, const char* type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
		}
	}
}

void Shader::SetBool(const char* name, bool value) const
{
	glUniform1i(m_UniformLookup.at(name), (int)value);
}

void Shader::SetInt(const const char* name, int value) const
{
	glUniform1i(m_UniformLookup.at(name), value);
}

void Shader::SetFloat(const const char* name, float value) const
{
	glUniform1f(m_UniformLookup.at(name), value);
}

void Shader::SetVec2(const const char* name, const glm::vec2& value) const
{
	glUniform2f(m_UniformLookup.at(name), value.x, value.y);
}

void Shader::SetVec2(const const char* name, float x, float y) const
{
	glUniform2f(m_UniformLookup.at(name), x, y);
}

void Shader::SetVec3(const const char* name, const glm::vec3& value) const
{
	glUniform3f(m_UniformLookup.at(name), value.x, value.y, value.z);
}

void Shader::SetVec3(const const char* name, float x, float y, float z) const
{

	glUniform3f(m_UniformLookup.at(name), x, y, z);
}

void Shader::SetVec4(const const char* name, const glm::vec4& value) const
{
	glUniform4f(m_UniformLookup.at(name), value.x, value.y, value.z, value.w);
}

void Shader::SetVec4(const const char* name, float x, float y, float z, float w)
{
	glUniform4f(m_UniformLookup.at(name), x, y, z, w);
}

void Shader::SetMat2(const const char* name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(m_UniformLookup.at(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat3(const const char* name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(m_UniformLookup.at(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetMat4(const const char* name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(m_UniformLookup.at(name), 1, GL_FALSE, &mat[0][0]);
}

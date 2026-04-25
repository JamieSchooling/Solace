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
		m_UniformLookup[name] = { location, (ShaderDataType)type }; // TODO: Implement check that type is supported
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

void Shader::SetUniform(const std::string& name, UniformData uniformData)
{
	if (std::holds_alternative<std::monostate>(uniformData))
	{
		std::cout << "Warning: Uniform value [" << name << "] not uploaded to shader, material value not set." << std::endl;
		return;
	}

	UniformDescription desc = m_UniformLookup.at(name);

	switch (desc.type)
	{
	case ShaderDataType::Bool:
		UploadUniform(desc.location, std::get<bool>(uniformData)); break;
	case ShaderDataType::Int:
		UploadUniform(desc.location, std::get<int>(uniformData)); break;
	case ShaderDataType::Float:
		UploadUniform(desc.location, std::get<float>(uniformData)); break;
	case ShaderDataType::Vector2:
		UploadUniform(desc.location, std::get<glm::vec2>(uniformData)); break;
	case ShaderDataType::Vector3:
		UploadUniform(desc.location, std::get<glm::vec3>(uniformData)); break;
	case ShaderDataType::Vector4:
		UploadUniform(desc.location, std::get<glm::vec4>(uniformData)); break;
	case ShaderDataType::Mat3:
		UploadUniform(desc.location, std::get<glm::mat3>(uniformData)); break;
	case ShaderDataType::Mat4:
		UploadUniform(desc.location, std::get<glm::mat4>(uniformData)); break;
	}
}

void Shader::UploadUniform(uint32_t location, bool value) const
{
	glUniform1i(location, (int)value);
}

void Shader::UploadUniform(uint32_t location, int value) const
{
	glUniform1i(location, value);
}

void Shader::UploadUniform(uint32_t location, float value) const
{
	glUniform1f(location, value);
}

void Shader::UploadUniform(uint32_t location, const glm::vec2& value) const
{
	glUniform2f(location, value.x, value.y);
}

void Shader::UploadUniform(uint32_t location, const glm::vec3& value) const
{
	glUniform3f(location, value.x, value.y, value.z);
}

void Shader::UploadUniform(uint32_t location, const glm::vec4& value) const
{
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::UploadUniform(uint32_t location, const glm::mat3& mat) const
{
	glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::UploadUniform(uint32_t location, const glm::mat4& mat) const
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

#include "Rendering/ShaderDataTypes.h"

ShaderDataType GLenumToShaderDataType(GLenum glEnum)
{
	switch (glEnum)
	{
	case GL_BOOL:
		return ShaderDataType::Bool;
	case GL_INT:
		return ShaderDataType::Int;
	case GL_FLOAT:
		return ShaderDataType::Float;

	case GL_FLOAT_VEC2:
		return ShaderDataType::Vector2;
	case GL_FLOAT_VEC3:
		return ShaderDataType::Vector3;
	case GL_FLOAT_VEC4:
		return ShaderDataType::Vector4;

	case GL_FLOAT_MAT3:
		return ShaderDataType::Matrix3;
	case GL_FLOAT_MAT4:
		return ShaderDataType::Matrix4;

	default:
		return ShaderDataType::Unknown;
	}
}

GLenum ShaderDataTypeToGLenum(ShaderDataType shaderType)
{
	switch (shaderType)
	{
	case ShaderDataType::Bool:
		return GL_BOOL;
	case ShaderDataType::Int:
		return GL_INT;
	case ShaderDataType::Float:
		return GL_FLOAT;

	case ShaderDataType::Vector2:
		return GL_FLOAT_VEC2;
	case ShaderDataType::Vector3:
		return GL_FLOAT_VEC3;
	case ShaderDataType::Vector4:
		return GL_FLOAT_VEC4;

	case ShaderDataType::Matrix3:
		return GL_FLOAT_MAT3;
	case ShaderDataType::Matrix4:
		return GL_FLOAT_MAT4;

	default:
		return 0;
	}
}

#pragma once

#include <glad/gl.h>

enum ShaderDataType
{
	Unknown,
	Bool,
	Short,
	UnsignedShort,
	Float,
	Int,
	UnsignedInt,
	Double,
	Vector2,
	Vector3,
	Vector4,
	Matrix3,
	Matrix4,
};

ShaderDataType GLenumToShaderDataType(GLenum glEnum);

GLenum ShaderDataTypeToGLenum(ShaderDataType shaderType);
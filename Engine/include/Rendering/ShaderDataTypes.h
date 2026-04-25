#pragma once

#include <glad/gl.h>

enum ShaderDataType
{
	Bool = GL_BOOL,
	Short = GL_SHORT,
	UnsignedShort = GL_UNSIGNED_SHORT,
	Float = GL_FLOAT,
	Int = GL_INT,
	UnsignedInt = GL_UNSIGNED_INT,
	Double = GL_DOUBLE,
	Vector2 = GL_FLOAT_VEC2,
	Vector3 = GL_FLOAT_VEC3,
	Vector4 = GL_FLOAT_VEC4,
	Mat3 = GL_FLOAT_MAT3,
	Mat4 = GL_FLOAT_MAT4,
};
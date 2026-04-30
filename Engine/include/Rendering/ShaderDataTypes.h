#pragma once

//#include <glad/gl.h>

enum ShaderDataType
{
	Bool = 0x8B56, //GL_BOOL
	Short = 0x1402, //GL_SHORT
	UnsignedShort = 0x1403, //GL_UNSIGNED_SHORT
	Float = 0x1406, //GL_FLOAT
	Int = 0x1404, //GL_INT
	UnsignedInt = 0x1405, //GL_UNSIGNED_INT
	Double = 0x140A, //GL_DOUBLE
	Vector2 = 0x8B50, //GL_FLOAT_VEC2
	Vector3 = 0x8B51, //GL_FLOAT_VEC3
	Vector4 = 0x8B52, //GL_FLOAT_VEC4
	Mat3 = 0x8B5B, //GL_FLOAT_MAT3
	Mat4 = 0x8B5C, //GL_FLOAT_MAT4
};
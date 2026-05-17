#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_vertexNormal;

uniform mat4 u_MVP;
uniform mat4 u_model;

out vec3 posWorldSpace;
out vec3 normal;

void main()
{
    posWorldSpace = (u_model * vec4(a_pos, 1.0)).xyz;
	normal = normalize(mat3(transpose(inverse(u_model))) * a_vertexNormal);
    gl_Position = u_MVP * vec4(a_pos, 1.0); 	
}
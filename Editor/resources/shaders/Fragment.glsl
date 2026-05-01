#version 460 core

layout(location = 0) out vec4 colour;

uniform vec3 u_Colour;

in vec3 vColour;

void main()
{
	colour = vec4(u_Colour * vColour, 1.0);
}
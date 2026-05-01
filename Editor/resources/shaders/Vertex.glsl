#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec3 posWorldSpace;
out vec3 vColour;

void main()
{
    posWorldSpace = (u_Model * vec4(aPos, 1.0)).xyz;
    vColour = aColour;
    gl_Position = u_MVP * vec4(aPos, 1.0); 	
}
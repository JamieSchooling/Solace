#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec3 posWorldSpace;

void main()
{
    posWorldSpace = (u_Model * vec4(aPos, 1.0)).xyz;
    gl_Position = u_MVP * vec4(aPos, 1.0); 	
}
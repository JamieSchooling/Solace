#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_vertexNormal;

layout (std140, binding = 0) uniform b_camera
{
    mat4 u_projection;
    mat4 u_view;
    vec3 u_viewPos;
};
uniform mat4 u_model;

mat4 MVP;

out vec3 posWorldSpace;
out vec3 normal;

void main()
{
    posWorldSpace = (u_model * vec4(a_pos, 1.0)).xyz;
	normal = normalize(mat3(transpose(inverse(u_model))) * a_vertexNormal);
    MVP = u_projection * u_view * u_model;
    gl_Position = MVP * vec4(a_pos, 1.0); 	
}
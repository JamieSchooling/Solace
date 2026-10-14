#version 460 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_vertexNormal;
layout (location = 2) in vec2 a_vertexUV;

layout (std140, binding = 0) uniform b_camera
{
    mat4 u_projection;
    mat4 u_view;
    vec3 u_viewPos;
};
uniform mat4 u_model;

const int MAX_SHADOW_CASCADES = 3;
uniform mat4 u_lightSpaceTransforms[MAX_SHADOW_CASCADES];
uniform float u_cascadeSplits[MAX_SHADOW_CASCADES];
uniform int u_numShadowCascades;

mat4 MVP;

out vec3 posWorldSpace;
out vec3 normal;
out vec2 texCoord;
out vec4 lightSpaceFragmentPositions[MAX_SHADOW_CASCADES];

void main()
{
    posWorldSpace = (u_model * vec4(a_pos, 1.0)).xyz;
    for (int i = 0; i < MAX_SHADOW_CASCADES; i++)
    {
        if (i >= u_numShadowCascades) break;
        
	    lightSpaceFragmentPositions[i] = u_lightSpaceTransforms[i] * vec4(posWorldSpace, 1.0);
    }
	normal = normalize(mat3(transpose(inverse(u_model))) * a_vertexNormal);
    MVP = u_projection * u_view * u_model;
    texCoord = a_vertexUV;
    gl_Position = MVP * vec4(a_pos, 1.0); 	
}
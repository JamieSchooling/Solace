#version 460 core

layout(location = 0) out vec4 colour;

struct directionalLight
{
	vec3 colour;
	vec3 direction;
};

layout (std140, binding = 0) uniform b_camera
{
    mat4 u_projection;
    mat4 u_view;
    vec3 u_viewPos;
};

layout (std140, binding = 1) uniform b_lights
{
	uniform directionalLight u_dLight;
};

uniform vec3 u_colour;

in vec3 posWorldSpace;
in vec3 normal;

vec3 getDirectionalLight()
{
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * u_dLight.colour;
	float diff = max(dot(normal, -u_dLight.direction), 0.0);
	vec3 diffuse = diff * u_dLight.colour;
	float specularStrength = 0.8;
	vec3 viewDir = normalize(u_viewPos - posWorldSpace);
	vec3 reflectDir = reflect(-u_dLight.direction, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * u_dLight.colour;      
	return ambient + diffuse + specular;
}

void main()
{
	colour = vec4(u_colour * getDirectionalLight(), 1.0);
}
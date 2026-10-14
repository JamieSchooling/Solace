#version 460 core

layout(location = 0) out vec4 colour;

struct DirectionalLight
{
	vec4 colour;
	vec4 direction;
	float intensity;
};

struct PointLight
{
	vec4 colour;
	vec4 position;
	float radius;
	float intensity;
};

layout (std140, binding = 0) uniform b_camera
{
    mat4 u_projection;
    mat4 u_view;
    vec3 u_viewPos;
};

const int MAX_POINT_LIGHTS = 32;
layout (std140, binding = 1) uniform b_lights
{
	DirectionalLight u_dLight;
	PointLight u_pLights[MAX_POINT_LIGHTS];
	int u_numPointLights;
};

uniform vec3 u_prop_colour;
uniform sampler2D u_prop_albedo;
uniform sampler2D u_prop_specular;
uniform float u_prop_specularAmount;

uniform sampler2DArray u_shadowMap;
const int MAX_SHADOW_CASCADES = 3;
uniform float u_cascadeSplits[MAX_SHADOW_CASCADES];
uniform int u_numShadowCascades;

in vec3 posWorldSpace;
in vec3 normal;
in vec2 texCoord;
in vec4 lightSpaceFragmentPositions[MAX_SHADOW_CASCADES];

float getShadowAmount()
{
	for (int i = 0; i < MAX_SHADOW_CASCADES; i++)
	{
        if (i >= u_numShadowCascades) break;

		vec4 fragPosViewSpace = u_view * vec4(posWorldSpace, 1.0);	
		float depthValue = abs(fragPosViewSpace.z);
		if (depthValue > u_cascadeSplits[i]) continue;

		float shadowAmount = 0.0;

		vec3 projCoords = lightSpaceFragmentPositions[i].xyz / lightSpaceFragmentPositions[i].w;
		projCoords = projCoords * 0.5 + 0.5;

		if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
		{
			return 0.0;
		}

		float closestDepth = textureLod(u_shadowMap, vec3(projCoords.xy, float(i)), 0).r; 
		float currentDepth = projCoords.z;

		float bias = 0.001;
		bias *= 1 / (u_cascadeSplits[i] * 0.5);
		if (currentDepth - bias > closestDepth) shadowAmount = 1.0;

		vec2 texelSize = (1.0 / textureSize(u_shadowMap, 0)).xy;
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = textureLod(u_shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, float(i)), 0).r; 
				shadowAmount += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}
		}
		shadowAmount /= 9.0;

		return shadowAmount;
//		return float(i);
	}
}

vec3 getDirectionalLight()
{
	float ambientStrength = 0.2 * u_dLight.intensity;
	vec3 ambient = ambientStrength * u_dLight.colour.rgb;
	float diff = max(dot(normal, -u_dLight.direction.rgb), 0.0);
	vec3 diffuse = diff * u_dLight.colour.rgb * u_dLight.intensity;
	float specularStrength = texture(u_prop_specular, texCoord).r * u_prop_specularAmount;
	vec3 viewDir = normalize(u_viewPos - posWorldSpace);
	vec3 reflectDir = reflect(u_dLight.direction.rgb, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = specularStrength * spec * u_dLight.colour.rgb * u_dLight.intensity;      
	return ambient + ((1.0 - getShadowAmount()) * (diffuse + specular));
}

vec3 getPointLight(int idx)
{
	vec3 delta = u_pLights[idx].position.xyz - posWorldSpace;

	float distSq = dot(delta, delta);
	float radius = u_pLights[idx].radius;

	if (distSq > radius * radius)
	{
		return vec3(0.0);
	}

	float distance = sqrt(distSq);

	vec3 lightDir = delta / distance;

	float x = distance / radius;
	float attn = 1.0 - x * x;
	attn *= attn;

	float diff = max(dot(normal, lightDir), 0.0);

	vec3 diffuse = diff * attn * u_pLights[idx].colour.rgb * u_pLights[idx].intensity;

	vec3 viewDir = normalize(u_viewPos - posWorldSpace);
	vec3 reflectDir = reflect(-lightDir, normal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
	
	float specularStrength = texture(u_prop_specular, texCoord).r * u_prop_specularAmount;
	vec3 specular = specularStrength * spec * attn * u_pLights[idx].colour.rgb * u_pLights[idx].intensity;
	
	return diffuse + specular;
}

void main()
{
	vec3 result = vec3(0.0);
	
	result += getDirectionalLight();

	for(int i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if (i >= u_numPointLights) break;

		result += getPointLight(i);
	}
	
	vec3 rgb = texture(u_prop_albedo, texCoord).rgb;
	colour = vec4(u_prop_colour * rgb * result, 1.0);

	
//	vec3 debugColours[3] = { vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0) };
//	colour = vec4(debugColours[int(getShadowAmount())], 1.0);
}
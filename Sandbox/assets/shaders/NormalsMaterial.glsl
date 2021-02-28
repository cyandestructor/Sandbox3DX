#type vertex
#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoords;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out vec2 texCoords;
out vec3 toLightVector;
out vec3 surfaceNormal;
out vec3 toCameraVector;
out vec3 lightDirection;

out float gl_ClipDistance[1];

uniform mat4 u_normalMatrix;
uniform mat4 u_viewProjection;
uniform mat4 u_transformation;

uniform vec3 u_cameraPosition;
uniform vec3 u_lightPosition;

uniform vec4 u_clipPlane;

void main()
{
	vec4 worldPosition = u_transformation * a_position;
	gl_Position = u_viewProjection * worldPosition;
	gl_ClipDistance[0] = dot(worldPosition, u_clipPlane);
	texCoords = a_texCoords;
	
	// TBN matrix
	vec3 T = normalize((u_normalMatrix * vec4(a_tangent, 0.0f)).xyz);
	vec3 B = normalize((u_normalMatrix * vec4(a_bitangent, 0.0f)).xyz);
	vec3 N = normalize((u_normalMatrix * vec4(a_normal, 0.0f)).xyz);
	mat3 TBN = transpose(mat3(T, B, N)); // To convert from world space to tangent space

	// Convert to tangent space
	toLightVector = TBN * normalize(u_lightPosition - worldPosition.xyz);
	lightDirection = -toLightVector;
	toCameraVector = TBN * normalize(u_cameraPosition - worldPosition.xyz);
}

#type pixel
#version 330 core
	
layout(location = 0) out vec4 o_color;

in vec2 texCoords;
in vec3 toLightVector;
in vec3 surfaceNormal;
in vec3 toCameraVector;
in vec3 lightDirection;

uniform vec4 u_lightColor;
uniform vec4 u_color;

uniform float u_ambientReduction;
uniform float u_diffuseReduction;
uniform float u_reflectivity;
uniform float u_shineDamper;

uniform sampler2D u_diffuseTex;
uniform sampler2D u_specularTex;
uniform sampler2D u_normalTex;

vec3 AmbientLight(vec3 lightColor, float reductionFactor);
vec3 DiffuseLight(vec3 toLightVector, vec3 normal, vec3 lightColor, float reductionFactor);
vec3 SpecularLight(vec3 lightDirection, vec3 normal, vec3 toCameraVector, vec3 lightColor, float reflectivity, float shineDamper, float reductionFactor);

void main()
{
	vec3 texNormal = texture(u_normalTex, texCoords).rgb;
	texNormal = normalize(texNormal * 2.0f - 1.0f);

	vec4 diffuseColor = texture(u_diffuseTex, texCoords) * u_color;

	vec4 texSpecular = texture(u_specularTex, texCoords);
	float specularReduction = texSpecular.r; // It can be any rgb component

	vec4 ambientLight = vec4(AmbientLight(u_lightColor.xyz, u_ambientReduction), 1.0f);
	vec4 diffuseLight = vec4(DiffuseLight(toLightVector, texNormal, u_lightColor.xyz, u_diffuseReduction), 1.0f);
	vec4 specularLight = vec4(SpecularLight(lightDirection, texNormal, toCameraVector, u_lightColor.xyz, u_reflectivity, u_shineDamper, specularReduction), 1.0f);

	o_color =  diffuseColor * (ambientLight + diffuseLight + specularLight);
}

vec3 AmbientLight(vec3 lightColor, float reductionFactor)
{
	vec3 ambientLight = lightColor * reductionFactor;
	return ambientLight;
}

vec3 DiffuseLight(vec3 toLightVector, vec3 normal, vec3 lightColor, float reductionFactor)
{
	float dotProduct = dot(normal, toLightVector);
	float brightness = max(dotProduct, 0.001f);
	vec3 diffuseLight = lightColor * brightness * reductionFactor;

	return diffuseLight;
}

vec3 SpecularLight(vec3 lightDirection, vec3 normal, vec3 toCameraVector, vec3 lightColor, float reflectivity, float shineDamper, float reductionFactor)
{
	vec3 reflectedLightDirection = reflect(lightDirection, normal);
	
	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.001f);

	float dampedFactor = pow(specularFactor, shineDamper);

	vec3 specularLight = lightColor * dampedFactor * reflectivity * reductionFactor;

	return specularLight;
}
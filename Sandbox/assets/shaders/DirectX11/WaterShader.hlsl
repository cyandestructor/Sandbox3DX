struct VS_INPUT
{
	float3 Position : a_position;
	float3 Normal : a_normal;
	float2 TexCoord : a_texCoords;
	float3 Tangent : a_tangent;
	float3 Bitangent : a_bitangent;
};

struct VS_OUTPUT
{
	float2 TexCoord : TEXCOORD0;
	float3 ToLightVector : LIGHT_VECTOR;
	float3 ToCameraVector : CAMERA_VECTOR;
	float3 LightDirection : LIGHT_DIRECTION;
	float4 ClipSpace : CLIPSPACE;
	float4 Position : SV_POSITION;
};

cbuffer VS_CB : register(b0)
{
	matrix u_viewProjection;
	matrix u_transformation;
	float3 u_cameraPosition;
	float3 u_lightPosition;
	float u_tilingFactor;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldPosition = mul(u_transformation, float4(input.Position, 1.0f));

	output.ClipSpace = mul(u_viewProjection, worldPosition);
	output.Position = mul(u_viewProjection, worldPosition);

	// This is because these vectors are given in RH coordinate system
	// The multiplication change them to LH for DirectX 11
	float3 lightPosition = u_lightPosition;
	lightPosition.z *= -1;

	float3 worldPositionRH = worldPosition.xyz;
	worldPositionRH.z *= -1;

	float3 cameraPosition = u_cameraPosition;
	cameraPosition.z *= -1;

	output.ToLightVector = normalize(lightPosition - worldPositionRH);
	output.LightDirection = -output.ToLightVector;
	output.ToCameraVector = normalize(cameraPosition - worldPositionRH);

	output.TexCoord = input.TexCoord * u_tilingFactor;

	return output;
}

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
	float3 ToLightVector : LIGHT_VECTOR;
	float3 ToCameraVector : CAMERA_VECTOR;
	float3 LightDirection : LIGHT_DIRECTION;
	float4 ClipSpace : CLIPSPACE;
};

cbuffer PS_CB : register(b0)
{
	float u_distortionFactor;
	float u_motionFactor;
	float u_reflectivity;
	float u_shineDamper;
	float u_ambientReduction;
	float4 u_lightColor;
	float4 u_color;
};

SamplerState TexSampler : register(s0);
Texture2D Reflection : register(t0);
Texture2D Refraction : register(t1);
Texture2D Distortion : register(t2);
Texture2D Normal : register(t3);

float4 AmbientLight(float4 lightColor, float reductionFactor);
float4 SpecularLight(float3 lightDirection, float3 normal, float3 toCameraVector, float4 lightColor,
	float reflectivity, float shineDamper);

float4 psMain(PS_INPUT input) : SV_Target
{
	float2 projectiveTexCoords = (input.ClipSpace.xy / input.ClipSpace.w) / 2.0f + 0.5f;
	float2 refractionTexCoords = projectiveTexCoords;
	float2 reflectionTexCoords = float2(projectiveTexCoords.x, -projectiveTexCoords.y);

	float4 distortionMap = Distortion.Sample(TexSampler, float2(input.TexCoord.x + u_motionFactor, input.TexCoord.y));
	float2 distortedTexCoords = distortionMap.rg * 0.1f;

	float4 reflection = Reflection.Sample(TexSampler, reflectionTexCoords);
	float4 refraction = Refraction.Sample(TexSampler, refractionTexCoords);

	return reflection;
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 AmbientLight(float4 lightColor, float reductionFactor)
{
	return lightColor * reductionFactor; // ambient light
}

float4 SpecularLight(float3 lightDirection, float3 normal, float3 toCameraVector, float4 lightColor,
	float reflectivity, float shineDamper)
{
	float3 reflectedLightDirection = reflect(lightDirection, normal);

	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.001f);

	float dampedFactor = pow(specularFactor, shineDamper);

	return lightColor * dampedFactor * reflectivity; // specular light
}
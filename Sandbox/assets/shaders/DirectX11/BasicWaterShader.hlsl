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

	output.Position = mul(u_viewProjection, worldPosition);
	output.TexCoord = input.TexCoord * u_tilingFactor;

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

	return output;
}

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
	float3 ToLightVector : LIGHT_VECTOR;
	float3 ToCameraVector : CAMERA_VECTOR;
	float3 LightDirection : LIGHT_DIRECTION;
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
Texture2D Diffuse : register(t0);
Texture2D Distortion : register(t1);
Texture2D Normal : register(t2);

float4 AmbientLight(float4 lightColor, float reductionFactor);
float4 SpecularLight(float3 lightDirection, float3 normal, float3 toCameraVector, float4 lightColor,
	float reflectivity, float shineDamper);

float4 psMain(PS_INPUT input) : SV_Target
{
	float4 distortionMap = Distortion.Sample(TexSampler, float2(input.TexCoord.x + u_motionFactor, input.TexCoord.y));
	float2 distortedTexCoords = distortionMap.rg * 0.1f;
	distortedTexCoords = input.TexCoord + float2(distortedTexCoords.x, distortedTexCoords.y + u_motionFactor);
	distortionMap = Distortion.Sample(TexSampler, distortedTexCoords);
	float2 distortion = (distortionMap.rg * 2.0f - 1.0f) * u_distortionFactor;

	float4 normalMap = Normal.Sample(TexSampler, distortedTexCoords);
	float3 normal = normalize(float3(normalMap.r * 2.0f - 1.0f, normalMap.b, normalMap.g * 2.0f - 1.0f));

	float4 ambientLight = AmbientLight(u_lightColor, u_ambientReduction);
	float4 specularLight = SpecularLight(input.LightDirection, normal, input.ToCameraVector, u_lightColor,
		u_reflectivity, u_shineDamper);

	float4 diffuseColor = Diffuse.Sample(TexSampler, distortedTexCoords);

	return diffuseColor * u_color * (ambientLight + specularLight);
}

float4 AmbientLight(float4 lightColor, float reductionFactor)
{
	return float4(lightColor.rgb * reductionFactor, 1.0f); // ambient light
}

float4 SpecularLight(float3 lightDirection, float3 normal, float3 toCameraVector, float4 lightColor,
	float reflectivity, float shineDamper)
{
	float3 reflectedLightDirection = reflect(lightDirection, normal);

	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.001f);

	float dampedFactor = pow(specularFactor, shineDamper);

	return float4(lightColor.rgb * dampedFactor * reflectivity, 1.0f); // specular light
}
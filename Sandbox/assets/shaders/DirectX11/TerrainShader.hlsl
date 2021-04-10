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
	float3 LightDirection : LIGHT_DIRECTION;
	float4 Position : SV_POSITION;
};

cbuffer VS_CB : register(b0)
{
	matrix u_viewProjection;
	matrix u_transformation;
	float3 u_lightPosition;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
	VS_OUTPUT output;

	float4 worldPosition = mul(u_transformation, float4(input.Position, 1.0f));

	output.Position = mul(u_viewProjection, worldPosition);
	output.TexCoord = input.TexCoord;

	// This is because these vectors are given in RH coordinate system
	// The multiplication change them to LH for DirectX 11
	input.Tangent.z *= -1;
	input.Bitangent.z *= -1;
	input.Normal.z *= -1;

	float3 lightPosition = u_lightPosition;
	lightPosition.z *= -1;

	float3 worldPositionRH = worldPosition.xyz;
	worldPositionRH.z *= -1;

	// TBN Matrix
	float3 T = normalize(mul(u_transformation, float4(input.Tangent, 0.0f)).xyz);
	float3 B = normalize(mul(u_transformation, float4(input.Bitangent, 0.0f)).xyz);
	float3 N = normalize(mul(u_transformation, float4(input.Normal, 0.0f)).xyz);
	float3x3 TBN = transpose(float3x3(T, B, N));

	// Convert to tangent space
	output.ToLightVector = mul(normalize(lightPosition - worldPositionRH), TBN);
	output.LightDirection = -output.ToLightVector;

	return output;
}

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
	float3 ToLightVector : LIGHT_VECTOR;
	float3 LightDirection : LIGHT_DIRECTION;
};

cbuffer PS_CB : register(b0)
{
	float u_repeatFactor;
	float u_ambientReduction;
	float u_diffuseReduction;
	float4 u_lightColor;
}

SamplerState texSampler : register(s0);

Texture2D blendMap : register(t0);
Texture2D diffuseTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D diffuseTexR : register(t3);
Texture2D normalTexR : register(t4);
Texture2D diffuseTexG : register(t5);
Texture2D normalTexG : register(t6);
Texture2D diffuseTexB : register(t7);
Texture2D normalTexB : register(t8);

float4 AmbientLight(float4 lightColor, float reductionFactor);
float4 DiffuseLight(float3 toLightVector, float3 normal, float4 lightColor, float reductionFactor);
float4 CalculateColorBlend(SamplerState samplerState,
	Texture2D blend, Texture2D bg,
	Texture2D red, Texture2D green, Texture2D blue,
	float2 texCoords, float repeat);

float4 psMain(PS_INPUT input) : SV_Target
{
	float4 normalColor = CalculateColorBlend(texSampler,
		blendMap, normalTex,
		normalTexR, normalTexG, normalTexB,
		input.TexCoord, u_repeatFactor);

	float3 texNormal = normalize(normalColor.rgb * 2.0f - 1.0f);

	float4 texColor = CalculateColorBlend(texSampler,
		blendMap, diffuseTex,
		diffuseTexR, diffuseTexG, diffuseTexB,
		input.TexCoord, u_repeatFactor);

	float4 ambientLight = AmbientLight(u_lightColor, u_ambientReduction);
	float4 diffuseLight = DiffuseLight(input.ToLightVector, texNormal, u_lightColor, u_diffuseReduction);

	return texColor * (ambientLight + diffuseLight);
}

float4 AmbientLight(float4 lightColor, float reductionFactor)
{
	return lightColor * reductionFactor; // ambient light
}

float4 DiffuseLight(float3 toLightVector, float3 normal, float4 lightColor, float reductionFactor)
{
	float dotProduct = dot(normal, toLightVector);
	float brightness = max(dotProduct, 0.001f);
	return lightColor * brightness * reductionFactor; // diffuse light
}

float4 CalculateColorBlend(SamplerState samplerState,
	Texture2D blend, Texture2D bg,
	Texture2D red, Texture2D green, Texture2D blue,
	float2 texCoords, float repeat)
{
	float2 tiledTexCoords = texCoords * repeat;
	
	float4 blendColor = blend.Sample(samplerState, texCoords);

	float bgAmount = 1 - (blendColor.r + blendColor.g + blendColor.b);

	float4 bgColor = bg.Sample(samplerState, tiledTexCoords) * bgAmount;
	float4 rColor = red.Sample(samplerState, tiledTexCoords) * blendColor.r;
	float4 gColor = green.Sample(samplerState, tiledTexCoords) * blendColor.g;
	float4 bColor = blue.Sample(samplerState, tiledTexCoords) * blendColor.b;

	return bgColor + rColor + gColor + bColor; // total color
}

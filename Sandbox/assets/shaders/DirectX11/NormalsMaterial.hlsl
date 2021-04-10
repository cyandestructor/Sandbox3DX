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
	float3 SurfaceNormal : NORMAL0;
	float3 ToCameraVector : CAMERA_VECTOR;
	float3 LightDirection : LIGHT_DIRECTION;
	float4 Position : SV_POSITION;
};

cbuffer VS_CB : register(b0)
{
	matrix u_viewProjection;
	matrix u_transformation;
	matrix u_normalMatrix;
	float3 u_cameraPosition;
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

	float3 cameraPosition = u_cameraPosition;
	cameraPosition.z *= -1;

	// TBN Matrix
	float3 T = normalize(mul(u_normalMatrix, float4(input.Tangent, 0.0f)).xyz);
	float3 B = normalize(mul(u_normalMatrix, float4(input.Bitangent, 0.0f)).xyz);
	float3 N = normalize(mul(u_normalMatrix, float4(input.Normal, 0.0f)).xyz);
	float3x3 TBN = transpose(float3x3(T, B, N));

	// Convert to tangent space
	output.ToLightVector = mul(normalize(lightPosition - worldPositionRH), TBN);
	output.LightDirection = -output.ToLightVector;
	output.ToCameraVector = mul(normalize(cameraPosition - worldPositionRH), TBN);

	return output;
}

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
	float3 ToLightVector : LIGHT_VECTOR;
	float3 SurfaceNormal : NORMAL0;
	float3 ToCameraVector : CAMERA_VECTOR;
	float3 LightDirection : LIGHT_DIRECTION;
};

cbuffer PS_CB : register(b0)
{
	float4 u_lightColor;
	float4 u_color;
	float u_ambientReduction;
	float u_diffuseReduction;
	float u_reflectivity;
	float u_shineDamper;
};

SamplerState texSampler : register(s0);
Texture2D diffuseTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D specularTex : register(t2);

float4 AmbientLight(float4 lightColor, float reductionFactor);
float4 DiffuseLight(float3 toLightVector, float3 normal, float4 lightColor, float reductionFactor);
float4 SpecularLight(float3 lightDirection, float3 normal, float3 toCameraVector, float4 lightColor,
	float reflectivity, float shineDamper, float reductionFactor);

float4 psMain(PS_INPUT input) : SV_Target
{
	float3 texNormal = normalTex.Sample(texSampler, input.TexCoord).rgb;
	texNormal = normalize(texNormal * 2.0f - 1.0f);

	float4 texSpecular = specularTex.Sample(texSampler, input.TexCoord);
	float specularReduction = texSpecular.r; // It can be any rgb component

	float4 diffuseColor = diffuseTex.Sample(texSampler, input.TexCoord) * u_color;

	float4 ambientLight = AmbientLight(u_lightColor, u_ambientReduction);
	float4 diffuseLight = DiffuseLight(input.ToLightVector, texNormal, u_lightColor, u_diffuseReduction);
	float4 specularLight = SpecularLight(input.LightDirection, texNormal, input.ToCameraVector, u_lightColor,
		u_reflectivity, u_shineDamper, specularReduction);

	return diffuseColor * (ambientLight + diffuseLight + specularLight);
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

float4 SpecularLight(float3 lightDirection, float3 normal, float3 toCameraVector, float4 lightColor,
	float reflectivity, float shineDamper, float reductionFactor)
{
	float3 reflectedLightDirection = reflect(lightDirection, normal);

	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.001f);

	float dampedFactor = pow(specularFactor, shineDamper);

	return lightColor * dampedFactor * reflectivity * reductionFactor; // specular light
}
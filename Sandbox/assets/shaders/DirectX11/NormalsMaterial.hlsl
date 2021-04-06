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
	//worldPosition.z *= -1;

	output.Position = mul(u_viewProjection, worldPosition);
	output.TexCoord = input.TexCoord;

	// TBN Matrix
	float3 T = normalize(mul(u_normalMatrix, float4(input.Tangent, 0.0f)).xyz);
	float3 B = normalize(mul(u_normalMatrix, float4(input.Bitangent, 0.0f)).xyz);
	float3 N = normalize(mul(u_normalMatrix, float4(input.Normal, 0.0f)).xyz);
	float3x3 TBN = transpose(float3x3(T, B, N));

	// Convert to tangent space
	output.ToLightVector = mul(TBN, normalize(u_lightPosition - worldPosition.xyz));
	output.LightDirection = -output.ToLightVector;
	output.ToCameraVector = mul(TBN, normalize(u_cameraPosition - worldPosition.xyz));

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

float4 psMain(PS_INPUT input) : SV_Target
{
	float4 diffuseColor = diffuseTex.Sample(texSampler, input.TexCoord) * u_color;
	return diffuseColor;
}
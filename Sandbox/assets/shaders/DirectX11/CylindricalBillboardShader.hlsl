struct VS_INPUT
{
	float3 Position : a_position;
	float2 TexCoord : a_texCoords;
};

struct VS_OUTPUT
{
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL0;
	float3 ToLightVector : LIGHT_VECTOR;
	float3 ToCameraVector : CAMERA_VECTOR;
	float3 LightDirection : LIGHT_DIRECTION;
	float4 Position : SV_POSITION;
};

cbuffer VS_CB : register(b0)
{
	matrix u_viewProjection;
	float3 u_cameraRightWS;
	float3 u_cameraUpWS;
	float3 u_billboardPosition;
	float3 u_billboardScale;

	float3 u_lightPosition;
	float3 u_cameraPosition;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
	VS_OUTPUT output;

	float3 upVector = float3(0.0f, 1.0f, 0.0f);
	float3 rightVector = u_cameraRightWS;

	output.Normal = normalize(cross(rightVector, upVector));
	float3 tangent = normalize(rightVector);
	float3 bitangent = upVector;

	// This is because these vectors are given in RH coordinate system
	// The multiplication change them to LH for DirectX 11
	tangent.z *= -1;
	bitangent.z *= -1;
	output.Normal.z *= -1;

	// TBN Matrix
	float3 T = tangent;
	float3 B = bitangent;
	float3 N = output.Normal;
	float3x3 TBN = transpose(float3x3(T, B, N));

	float3 worldPosition =
		u_billboardPosition
		+ rightVector * input.Position.x * u_billboardScale.x
		+ upVector * input.Position.y * u_billboardScale.y;

	output.Position = mul(u_viewProjection, float4(worldPosition, 1.0f));
	output.TexCoord = input.TexCoord;

	float3 lightPosition = u_lightPosition;
	lightPosition.z *= -1;

	float3 worldPositionRH = worldPosition.xyz;
	worldPositionRH.z *= -1;

	float3 cameraPosition = u_cameraPosition;
	cameraPosition.z *= -1;

	// Convert to tangent space
	output.ToLightVector = mul(normalize(lightPosition - worldPositionRH), TBN);
	output.LightDirection = -output.ToLightVector;
	output.ToCameraVector = mul(normalize(cameraPosition - worldPositionRH), TBN);

	return output;
}

struct PS_INPUT
{
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL0;
	float3 ToLightVector : LIGHT_VECTOR;
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

	float4 finalColor = diffuseColor * (ambientLight + diffuseLight + specularLight);

	clip(finalColor.a < 0.5f ? -1 : 1);

	return finalColor;
}

float4 AmbientLight(float4 lightColor, float reductionFactor)
{
	return float4(lightColor.rgb * reductionFactor, 1.0f); // ambient light
}

float4 DiffuseLight(float3 toLightVector, float3 normal, float4 lightColor, float reductionFactor)
{
	float dotProduct = dot(normal, toLightVector);
	float brightness = max(dotProduct, 0.001f);
	return float4(lightColor.rgb * brightness * reductionFactor, 1.0f); // diffuse light
}

float4 SpecularLight(float3 lightDirection, float3 normal, float3 toCameraVector, float4 lightColor,
	float reflectivity, float shineDamper, float reductionFactor)
{
	float3 reflectedLightDirection = reflect(lightDirection, normal);

	float specularFactor = dot(reflectedLightDirection, toCameraVector);
	specularFactor = max(specularFactor, 0.001f);

	float dampedFactor = pow(specularFactor, shineDamper);

	return float4(lightColor.rgb * dampedFactor * reflectivity * reductionFactor, 1.0f); // specular light
}
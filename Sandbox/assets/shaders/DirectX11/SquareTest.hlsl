struct VS_INPUT {
	float3 position : a_position;
	float2 texCoord : a_texCoord;
};

struct VS_OUTPUT {
	float2 texCoord : TexCoord;
	float4 position : SV_Position;
};

cbuffer VS_CB {
	matrix u_viewProjection;
	matrix u_transformation;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
	VS_OUTPUT output;

	matrix mvp = mul(u_viewProjection, u_transformation);

	output.position = mul(mvp, float4(input.position, 1.0f));
	output.texCoord = input.texCoord;

	return output;
}

struct PS_INPUT {
	float2 texCoord : TexCoord;
};

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

float4 psMain(PS_INPUT input) : SV_Target
{
	return tex.Sample(texSampler, input.texCoord);
}
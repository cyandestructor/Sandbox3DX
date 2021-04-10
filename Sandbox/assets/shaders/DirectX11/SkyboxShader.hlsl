struct VS_INPUT {
	float3 Position : a_position;
};

struct VS_OUTPUT {
	float3 TexCoords : TexCoords;
	float4 Position : SV_Position;
};

cbuffer VS_CB : register(b0)
{
	matrix u_viewProjection;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
	VS_OUTPUT output;

	output.TexCoords = input.Position;

	output.Position = mul(u_viewProjection, float4(input.Position, 1.0f));
	output.Position = output.Position.xyww;

	return output;
}

struct PS_INPUT
{
	float3 TexCoords : TexCoords;
};

cbuffer PS_CB : register(b0)
{
	float u_blendLevel;
	int u_texture;
	int u_texture2;
};

SamplerState texSampler : register(s1);
TextureCube Textures[2];

float4 GetTexture3DColor(int index, float3 texCoords);

float4 psMain(PS_INPUT input) : SV_Target
{
	float4 color1 = GetTexture3DColor(u_texture, input.TexCoords);
	float4 color2 = GetTexture3DColor(u_texture2, input.TexCoords);

	return lerp(color1, color2, u_blendLevel); // Total color
}

float4 GetTexture3DColor(int index, float3 texCoords)
{
	if (index == 0) {
		return Textures[0].Sample(texSampler, texCoords);
	}
	else if (index == 1) {
		return Textures[1].Sample(texSampler, texCoords);
	}

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
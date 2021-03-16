struct VS_INPUT {
	float3 position : a_position;
	float4 color : a_color;
};

struct VS_OUTPUT {
	float4 color : Color0;
	float4 position : SV_Position;
};

VS_OUTPUT vsMain(VS_INPUT input)
{
	VS_OUTPUT output;

	output.position = float4(input.position, 1.0f);
	output.color = input.color;

	return output;
}

struct PS_INPUT {
	float4 color : Color0;
};

float3 psMain(PS_INPUT input) : SV_Target
{
	return input.color;
}
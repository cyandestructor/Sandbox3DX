struct VS_INPUT {
	float3 position : a_position;
	float4 color : a_color;
};

struct VS_OUTPUT {
	float4 color : Color0;
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
	output.color = input.color;

	return output;
}

struct PS_INPUT {
	float4 color : Color0;
};

float4 psMain(PS_INPUT input) : SV_Target
{
	return input.color;
}
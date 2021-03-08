// Skybox vertex shader

cbuffer MatrixBuffer : register(b0)
{
	matrix projectionMatrix;
};

struct OutputType
{
	float3 worldPosition : Position;
	float4 position : SV_POSITION;
};

OutputType main(float3 position: Position)
{
	OutputType output;

	output.worldPosition = position;
	output.position = mul(float4(position, 0.0f), projectionMatrix);

	// make sure that the depth is set to 1 (so that the z-buffering will work)
	output.position.z = output.position.w;

	return output;
}
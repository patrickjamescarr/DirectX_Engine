// Skybox pixel shader

TextureCube textureCube : register(t0);
SamplerState SampleType : register(s0);

float4 main(float3 worldPosition : Position) : SV_TARGET
{
	return textureCube.Sample(SampleType, worldPosition);
}


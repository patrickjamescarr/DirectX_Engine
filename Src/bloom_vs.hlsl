// bloom vertex shader
// Simple texture pass

struct InputType
{
	float2 tex : TEXCOORD0;
    float4 color    : COLOR0;
    float4 pos : POSITION;
};

struct OutputType
{
    float2 tex : TEXCOORD0;
    float4 color : COLOR0;
    float4 pos   : SV_POSITION;
};

OutputType main(InputType input)
{
	OutputType output;


    output.pos = input.pos;

	output.tex = input.tex;

	return output;
}
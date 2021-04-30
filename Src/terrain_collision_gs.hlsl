#include "density.hlsli"

struct GSOutput
{
	float4 pos : SV_POSITION;
    float4 camPos : CAMPOSITION;
    float4 densities : DENSITY;
    float4 densities2 : DENSITY1;
};

float SampleVector(float3 v)
{
    float d;

    float length = 1;

    for (int i = 0; i < 4; i++)
    {
        d = DENSITY(v * length);

        if (d >= 0) return d;

        length -= 0.25f;
    }

    return d;
}

float3 direction(float3 a, float3 b)
{
    return a + ((b * 0.5) / 0.1f);
}


[maxvertexcount(1)]
void main(
	point float4 input[1] : SV_POSITION, 
	inout PointStream< GSOutput > output
)
{

    float d = DENSITY(input[0].xyz);

    // xz axis
    float forward = DENSITY(direction(input[0].xyz, float3(0, 0, 1)));
    float forwardRight = DENSITY(direction(input[0].xyz, normalize(float3(1, 0, 1))));
    float right = DENSITY(direction(input[0].xyz, float3(1, 0, 0)));
    float backRight = DENSITY(direction(input[0].xyz, normalize(float3(1, 0, -1))));
    float back = DENSITY(direction(input[0].xyz, float3(0, 0, -1)));
    float backLeft = DENSITY(direction(input[0].xyz, normalize(float3(-1, 0, -1))));
    float left = DENSITY(direction(input[0].xyz, float3(-1, 0, 0)));
    float forwardLeft = DENSITY(direction(input[0].xyz, normalize(float3(-1, 0, 1))));

    //xy axis
    // 1. up, 2. up & forward, 3. up & back, 4. down, 5. down & forward, 6. down & back


    GSOutput element;
    element.pos = float4(d, d < 0, DENSITY(input[0].xyz), 0);
    element.camPos = input[0];
    element.densities = float4(forward, forwardLeft, left, backLeft);
    element.densities2 = float4(back, backRight, right, forwardRight);
    output.Append(element);
}
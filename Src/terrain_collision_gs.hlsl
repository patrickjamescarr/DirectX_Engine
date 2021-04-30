#include "density.hlsli"

struct GSOutput
{
	float4 pos : SV_POSITION;
    float4 camPos : CAMPOSITION;
    float4 densities : DENSITY;
    float4 densities2 : DENSITY1;
    float4 densities3 : DENSITY2;
    float4 densities4 : DENSITY3;
    float4 densities5 : DENSITY4;
    float4 densities6 : DENSITY5;
    float4 densities7 : DENSITY6;
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
    return a + ((b * 0.5) / 0.3f);
}


[maxvertexcount(1)]
void main(
	point float4 input[1] : SV_POSITION, 
	inout PointStream< GSOutput > output
)
{

    float d = DENSITY(input[0].xyz);

    // horizontal movement
    float forward = DENSITY(direction(input[0].xyz, float3(0, 0, 1)));
    float forwardRight = DENSITY(direction(input[0].xyz, normalize(float3(1, 0, 1))));
    float right = DENSITY(direction(input[0].xyz, float3(1, 0, 0)));
    float backRight = DENSITY(direction(input[0].xyz, normalize(float3(1, 0, -1))));
    float back = DENSITY(direction(input[0].xyz, float3(0, 0, -1)));
    float backLeft = DENSITY(direction(input[0].xyz, normalize(float3(-1, 0, -1))));
    float left = DENSITY(direction(input[0].xyz, float3(-1, 0, 0)));
    float forwardLeft = DENSITY(direction(input[0].xyz, normalize(float3(-1, 0, 1))));

    // moving up
    float up = DENSITY(direction(input[0].xyz, float3(0, -1, 0)));
    float forwardUp = DENSITY(direction(input[0].xyz, normalize(float3(0, -1, 1))));
    float forwardRightUp = DENSITY(direction(input[0].xyz, normalize(float3(1, -1, 1))));
    float rightUp = DENSITY(direction(input[0].xyz, float3(1, -1, 0)));
    float backRightUp = DENSITY(direction(input[0].xyz, normalize(float3(1, -1, -1))));
    float backUp = DENSITY(direction(input[0].xyz, float3(0, -1, -1)));
    float backLeftUp = DENSITY(direction(input[0].xyz, normalize(float3(-1, -1, -1))));
    float leftUp = DENSITY(direction(input[0].xyz, float3(-1, -1, 0)));
    float forwardLeftUp = DENSITY(direction(input[0].xyz, normalize(float3(-1, -1, 1))));

    // moving down
    float down = DENSITY(direction(input[0].xyz, float3(0, 1, 0)));
    float forwardDown = DENSITY(direction(input[0].xyz, normalize(float3(0, 1, 1))));
    float forwardRightDown = DENSITY(direction(input[0].xyz, normalize(float3(1, 1, 1))));
    float rightDown = DENSITY(direction(input[0].xyz, float3(1, 1, 0)));
    float backRightDown = DENSITY(direction(input[0].xyz, normalize(float3(1, 1, -1))));
    float backDown = DENSITY(direction(input[0].xyz, float3(0, 1, -1)));
    float backLeftDown = DENSITY(direction(input[0].xyz, normalize(float3(-1, 1, -1))));
    float leftDown = DENSITY(direction(input[0].xyz, float3(-1, 1, 0)));
    float forwardLeftDown = DENSITY(direction(input[0].xyz, normalize(float3(-1, 1, 1))));

    GSOutput element;
    element.pos = float4(d, d < 0, DENSITY(input[0].xyz), 0);
    element.camPos = input[0];
    element.densities = float4(forward, forwardLeft, left, backLeft);
    element.densities2 = float4(back, backRight, right, forwardRight);
    element.densities3 = float4(up, forwardUp, forwardRightUp, rightUp);
    element.densities4 = float4(backRightUp, backUp, backLeftUp, leftUp);
    element.densities5 = float4(forwardLeftUp, forwardDown, forwardRightDown, rightDown);
    element.densities6 = float4(backRightDown, backDown, backLeftDown, leftDown);
    element.densities7 = float4(forwardLeftDown, down, 0, 0);
    output.Append(element);
}
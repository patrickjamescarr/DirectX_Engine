Texture2D<float4> Texture : register(t0);
sampler TextureSampler : register(s0);

cbuffer FlareBuffer : register(b0)
{
    float brightness;
}

float4 main(float4 backgroundColour : COLOR0, float2 texCoord : TEXCOORD0) : SV_Target0
{
    float4 textureColour = Texture.Sample(TextureSampler, texCoord);

    // set the transparency level
    textureColour.a *= 0.4;

    // blend the texture's transparaent background colour with the 
    // texture colour using our defined transparency level above
    float3 blend = (textureColour.rgb * textureColour.a) + (backgroundColour.rgb * (1 - textureColour.a));

    // populate the blend result to return.
    float4 blendResult;
    blendResult.rgb = blend.rgb;
    blendResult.a = textureColour.a;

    // finally set the brighness.
    blendResult.rgb += (brightness / 2);

    return blendResult;
}
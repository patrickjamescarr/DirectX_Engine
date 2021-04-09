#include "simplex_3d.hlsli" 

float DENSITY(float3 ws)
{
//    float density = -ws.y;
//
//    density += noise(ws) * 10.0f;
//
//    return density;

    float m_frequency = 1.01f;
    float m_amplitude = 1.f;
    float m_gain = 0.487f;
    int m_octaves = 4;


    float density = -ws.y;

    density += snoise(ws) * 10.0f;

    //float hard_floor_y =1.f;

    //density += clamp(((hard_floor_y - ws.y) * 3) * 40.0f, 0.f, 1.f);

    ////density += m_simplex.Generate(x*1.01, y*1.01, z*1.01)*1.00;

    ////density += m_simplex.Generate(x*1.96, y*1.96, z*1.96)*0.50;

    ////density += m_simplex.Generate(x*4.03, y*4.03, z*4.03)*0.25;

    //float frequency = m_frequency;
    //float amplitude = m_amplitude;

    //for (int i = 0; i < m_octaves; i++)
    //{
    //    float3 p = float3(ws.x, ws.y, ws.z);
    //    density += noise(p * frequency)*amplitude;
    //    amplitude *= 0.5;
    //    frequency *= 1.945;
    //}



    return density;
}
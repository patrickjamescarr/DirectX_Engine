#pragma once
#include "Bindable.h"
#include "VertexConstantBuffer.h"

class MarchingCubesGenerateConstantBufferVS :
    public Bindable
{
private:
    // Buffer to store the cube variables
    // x val contains cube scale
    // y val contains cube dimentions
    struct CubeBufferType
    {
        DirectX::XMFLOAT4 cubeVariables;
    };
public:
    MarchingCubesGenerateConstantBufferVS(DX::DeviceResources& deviceResources, const int* dimention, const float* scale, UINT slot = 0);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<VertexConstantBuffer<CubeBufferType>> m_vertexConstantBuffer;
    const float* m_scale;
    const int* m_dimention;
};
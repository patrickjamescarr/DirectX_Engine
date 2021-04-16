#pragma once
#include "Bindable.h"
#include "GameObject.h"
#include "VertexConstantBuffer.h"

class MarchingCubesDensityConstantBufferVS :
    public Bindable
{
private:
    //standard matrix buffer supplied to all shaders
    struct MatrixBufferType
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };
public:
    MarchingCubesDensityConstantBufferVS(
        DX::DeviceResources& deviceResources, 
        const DirectX::SimpleMath::Matrix& transform,
        const float &cubeDimention, 
        const int& xPos, 
        const int& yPos);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<VertexConstantBuffer<MatrixBufferType>> m_vertexConstantBuffer;
    float m_cubeDimention;
    int m_xPos;
    int m_yPos;
    DirectX::SimpleMath::Matrix m_transform;
};


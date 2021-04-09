#pragma once
#include "Bindable.h"
#include "GameObject.h"
#include "VertexConstantBuffer.h"

class MarchingCubesConstantBufferVS :
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
    MarchingCubesConstantBufferVS(DX::DeviceResources& deviceResources, const GameObject& parent, const float &cubeDimention);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<VertexConstantBuffer<MatrixBufferType>> m_vertexConstantBuffer;
    const GameObject& m_parent;
    float m_cubeDimention;
};


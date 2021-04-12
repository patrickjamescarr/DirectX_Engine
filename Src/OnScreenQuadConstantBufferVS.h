#pragma once
#include "Bindable.h"
#include "GameObject.h"
#include "VertexConstantBuffer.h"

class OnScreenQuadConstantBufferVS :
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
    OnScreenQuadConstantBufferVS(DX::DeviceResources& deviceResources, const GameObject& parent, UINT slot = 0);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<VertexConstantBuffer<MatrixBufferType>> m_vertexConstantBuffer;
    const GameObject& m_parent;
    DirectX::SimpleMath::Matrix m_view;
};


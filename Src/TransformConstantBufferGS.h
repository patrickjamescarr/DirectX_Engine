#pragma once
#include "Bindable.h"
#include "GameObject.h"
#include "GeometryConstantBuffer.h"

class TransformConstantBufferGS :
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
    TransformConstantBufferGS(DX::DeviceResources& deviceResources, const GameObject& parent);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<GeometryConstantBuffer<MatrixBufferType>> m_geometryConstantBuffer;
    const GameObject& m_parent;
};


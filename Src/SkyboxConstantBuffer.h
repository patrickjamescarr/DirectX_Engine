#pragma once
#include "Bindable.h"
#include "DeviceResources.h"
#include "VertexConstantBuffer.h"

class SkyboxConstantBuffer :
    public Bindable
{
private:
    struct MatrixBufferType
    {
        DirectX::XMMATRIX projection;
    };
public:
    SkyboxConstantBuffer(DX::DeviceResources& deviceResources);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<VertexConstantBuffer<MatrixBufferType>> m_vertexConstantBuffer;
};


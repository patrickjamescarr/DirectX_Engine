#pragma once
#include "Bindable.h"
#include "GeometryConstantBuffer.h"
#include "VertexConstantBuffer.h"
#include "PixelConstantBuffer.h"
#include "Camera.h"
#include "Types.h"

class FogConstantBuffer :
    public Bindable
{
private:
    struct FogBufferType
    {
        DirectX::SimpleMath::Vector4 fogDistance;
        DirectX::SimpleMath::Vector4 cameraPosition;
    };
public:
    FogConstantBuffer(
        DX::DeviceResources& deviceResources,
        const float* end, 
        const Camera* camera,
        ShaderType shaderType,
        UINT slot = 0
    );
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    std::unique_ptr<GeometryConstantBuffer<FogBufferType>> m_geometryBuffer;
    std::unique_ptr<VertexConstantBuffer<FogBufferType>> m_vertexBuffer;
    std::unique_ptr<PixelConstantBuffer<FogBufferType>> m_pixelBuffer;
    const float* m_start;
    const float* m_end;
    const Camera* m_camera;
    ShaderType m_shaderType;
};


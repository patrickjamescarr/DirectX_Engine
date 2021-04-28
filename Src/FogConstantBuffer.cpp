#include "pch.h"
#include "FogConstantBuffer.h"

using namespace DirectX::SimpleMath;

FogConstantBuffer::FogConstantBuffer(
    DX::DeviceResources& deviceResources, 
    const float* end, 
    const Camera* camera,
    ShaderType shaderType,
    UINT slot
)
    : m_end(end), m_camera(camera), m_shaderType(shaderType)
{
    switch (shaderType)
    {
        case ShaderType::Vertex:
            m_vertexBuffer = std::make_unique<VertexConstantBuffer<FogBufferType>>(deviceResources, slot);
            break;
        case ShaderType::Geometry:
            m_geometryBuffer = std::make_unique<GeometryConstantBuffer<FogBufferType>>(deviceResources, slot);
            break;
        case ShaderType::Pixel:
            m_pixelBuffer = std::make_unique<PixelConstantBuffer<FogBufferType>>(deviceResources, slot);
            break;
    default:
        break;
    }
}

void FogConstantBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
    const FogBufferType fogBuffer
    {
        Vector4(*m_end, 0, 0, 0),
        (Vector4)m_camera->getPosition()
    };

    switch (m_shaderType)
    {
    case ShaderType::Vertex:
        m_vertexBuffer->Update(deviceResources, fogBuffer);
        m_vertexBuffer->Bind(deviceResources);
        break;
    case ShaderType::Geometry:
        m_geometryBuffer->Update(deviceResources, fogBuffer);
        m_geometryBuffer->Bind(deviceResources);
        break;
    case ShaderType::Pixel:
        m_pixelBuffer->Update(deviceResources, fogBuffer);
        m_pixelBuffer->Bind(deviceResources);
        break;
    default:
        break;
    }
}

#include "pch.h"
#include "LensFlareConstantBuffer.h"

LensFlareConstantBuffer::LensFlareConstantBuffer(DX::DeviceResources & deviceResources)
    : m_deviceResources(deviceResources)
{
    m_value.brightness = 0;

    if (!m_flareBuffer)
    {
        m_flareBuffer = std::make_unique<PixelConstantBuffer<FlareBufferType>>(deviceResources);
    }
}

void LensFlareConstantBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
    m_flareBuffer->Update(deviceResources, m_value);
    m_flareBuffer->Bind(deviceResources);
}

void LensFlareConstantBuffer::Bind() noexcept
{
    m_flareBuffer->Update(m_deviceResources, m_value);
    m_flareBuffer->Bind(m_deviceResources);
}

#include "pch.h"
#include "GaussianBlurConstantBuffer.h"

GaussianBlurConstantBuffer::GaussianBlurConstantBuffer(DX::DeviceResources & deviceResources, BlurBufferType blurParams)
    : m_blurParams(blurParams), m_deviceResources(deviceResources)
{
    if (!m_blurBuffer)
    {
        m_blurBuffer = std::make_unique<PixelConstantBuffer<BlurBufferType>>(deviceResources);
    }
}

void GaussianBlurConstantBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
    m_blurBuffer->Update(deviceResources, m_blurParams);
    m_blurBuffer->Bind(deviceResources);
}

void GaussianBlurConstantBuffer::Set()
{
    m_blurBuffer->Update(m_deviceResources, m_blurParams);
    m_blurBuffer->Bind(m_deviceResources);
}

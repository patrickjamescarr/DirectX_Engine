#include "pch.h"
#include "BloomConstantBuffer.h"

BloomConstantBuffer::BloomConstantBuffer(DX::DeviceResources & deviceResources) 
	: m_deviceResources(deviceResources)
{
	if (!m_bloomBuffer)
	{
		m_bloomBuffer = std::make_unique<PixelConstantBuffer<BloomBufferType>>(deviceResources);
	}
}

void BloomConstantBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
	m_bloomBuffer->Update(deviceResources, bloomSetting);
	m_bloomBuffer->Bind(deviceResources);
}

void BloomConstantBuffer::Set()
{
    m_bloomBuffer->Update(m_deviceResources, bloomSetting);
    m_bloomBuffer->Bind(m_deviceResources);
}

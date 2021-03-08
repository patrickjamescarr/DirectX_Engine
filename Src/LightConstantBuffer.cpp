#include "pch.h"
#include "LightConstantBuffer.h"

LightConstantBuffer::LightConstantBuffer(DX::DeviceResources & deviceResources, Light *sceneLight)
	: m_sceneLight(sceneLight)
{
	if (!m_lightBuffer)
	{
		m_lightBuffer = std::make_unique<PixelConstantBuffer<LightBufferType>>(deviceResources);
	}
}

void LightConstantBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
	const LightBufferType lightBuffer 
	{
		m_sceneLight->getAmbientColour(),
		m_sceneLight->getDiffuseColour(),
		m_sceneLight->getPosition(),
		0.0f
	};

	m_lightBuffer->Update(deviceResources, lightBuffer);
	m_lightBuffer->Bind(deviceResources);
}

#include "pch.h"
#include "PixelShader.h"
#include <d3dcompiler.h>

PixelShader::PixelShader(DX::DeviceResources & deviceResources, const  WCHAR * fileName)
    : m_deviceResources(deviceResources)
{
	auto pixelShaderBuffer = DX::ReadData(fileName);

	DX::ThrowIfFailed(
		deviceResources.GetD3DDevice()->CreatePixelShader(
			pixelShaderBuffer.data(), 
			pixelShaderBuffer.size(), 
			nullptr, 
			&m_pixelShader
		)
	);
}

void PixelShader::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

void PixelShader::Set()
{
    m_deviceResources.GetD3DDeviceContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

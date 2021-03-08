#include "pch.h"
#include "VertexShader.h"
#include <d3dcompiler.h>

VertexShader::VertexShader(DX::DeviceResources& deviceResources, const WCHAR * fileName)
{
	m_bytecode = std::make_unique<std::vector<uint8_t>>(DX::ReadData(fileName));

	DX::ThrowIfFailed(
		deviceResources.GetD3DDevice()->CreateVertexShader(
			m_bytecode->data(),
			m_bytecode->size(),
			NULL, &m_vertexShader
		)
	);
}

void VertexShader::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->VSSetShader(m_vertexShader.Get(), 0, 0);
}

std::vector<uint8_t>* VertexShader::GetBytecode() const noexcept
{
	return m_bytecode.get();
}

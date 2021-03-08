#include "pch.h"
#include "InputLayout.h"

InputLayout::InputLayout(DX::DeviceResources & deviceResources, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, std::vector<uint8_t>* vertexShaderBytecode)
{
 	DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateInputLayout(
		layout.data(), (UINT)layout.size(), 
		vertexShaderBytecode->data(), 
		vertexShaderBytecode->size(), 
		&m_inputLayout
	));
}

void InputLayout::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->IASetInputLayout(m_inputLayout.Get());
}

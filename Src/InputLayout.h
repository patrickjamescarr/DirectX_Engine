#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(DX::DeviceResources& deviceResources, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, std::vector<uint8_t>* vertexShaderBytecode);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};


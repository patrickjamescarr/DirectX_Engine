#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class VertexShader : public Bindable
{
public:
	VertexShader(DX::DeviceResources& deviceResources, const WCHAR * fileName);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
	std::vector<uint8_t>* GetBytecode() const noexcept;
protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
	std::unique_ptr<std::vector<uint8_t>> m_bytecode;
};


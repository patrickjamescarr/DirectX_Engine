#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class RasterizerState :
	public Bindable
{
public:
	RasterizerState(DX::DeviceResources& deviceResources, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

};


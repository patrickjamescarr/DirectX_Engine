#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class Sampler :
	public Bindable
{
public:
	Sampler(DX::DeviceResources& deviceResources, D3D11_TEXTURE_ADDRESS_MODE mode, int MaxAnisotropy = D3D11_REQ_MAXANISOTROPY);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
};


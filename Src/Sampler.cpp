#include "pch.h"
#include "Sampler.h"

Sampler::Sampler(DX::DeviceResources & deviceResources, D3D11_TEXTURE_ADDRESS_MODE mode, int MaxAnisotropy)
{
	//Create a texture sampler state description.
	D3D11_SAMPLER_DESC	samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = mode;
	samplerDesc.AddressV = mode;
	samplerDesc.AddressW = mode;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = MaxAnisotropy;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf()));
}

void Sampler::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
}

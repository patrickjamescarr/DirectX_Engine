#include "pch.h"
#include "RasterizerState.h"

RasterizerState::RasterizerState(DX::DeviceResources & deviceResources, D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterizerDesc.CullMode = cullMode;
    rasterizerDesc.FillMode = fillMode;

	DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState));
}

void RasterizerState::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->RSSetState(m_rasterizerState.Get());
}

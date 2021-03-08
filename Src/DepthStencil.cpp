#include "pch.h"
#include "DepthStencil.h"

DepthStencil::DepthStencil(DX::DeviceResources & deviceResources)
{
	auto width = deviceResources.GetOutputSize().right;
	auto height = deviceResources.GetOutputSize().bottom;

	// Set up the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	DX::ThrowIfFailed(
		deviceResources.GetD3DDevice()->CreateTexture2D(
			&depthBufferDesc,
			nullptr, 
			depthStencilBuffer.ReleaseAndGetAddressOf()
		)
	);

	// Set up the depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	DX::ThrowIfFailed(
		deviceResources.GetD3DDevice()->CreateDepthStencilView(
			depthStencilBuffer.Get(), 
			&depthStencilViewDesc, 
			m_depthStencilView.ReleaseAndGetAddressOf()
		)
	);
}

void DepthStencil::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, deviceResources.GetRenderTargetViewAddress(), m_depthStencilView.Get());
}

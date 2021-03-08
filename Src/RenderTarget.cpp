#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(DX::DeviceResources & deviceResources)
{
}

void RenderTarget::Bind(DX::DeviceResources & deviceResources) noexcept
{
	// getting the RTV address like this isnt great. refactor for a more generic approach
	deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, deviceResources.GetRenderTargetViewAddress(), deviceResources.GetDepthStencilView());

	// get dimentions from the render target
	D3D11_TEXTURE2D_DESC textureDesc;
	deviceResources.GetRenderTarget()->GetDesc(&textureDesc);

	// configure the viewport
	D3D11_VIEWPORT viewPort;
	viewPort.Width = (float)textureDesc.Width;
	viewPort.Height = (float)textureDesc.Height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	deviceResources.GetD3DDeviceContext()->RSSetViewports(1u, &viewPort);
}

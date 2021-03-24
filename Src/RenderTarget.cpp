#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(DX::DeviceResources & deviceResources)
{
    auto width = deviceResources.GetBackBufferWidth();
    auto height = deviceResources.GetBackBufferHeight();
    auto backBufferFormat = deviceResources.GetBackBufferFormat();

    CD3D11_TEXTURE2D_DESC rtDesc(backBufferFormat, width / 2, height / 2,
        1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateTexture2D(&rtDesc, nullptr,
        m_renderTexture.GetAddressOf()));

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRenderTargetView(m_renderTexture.Get(), nullptr,
        m_renderTarget.ReleaseAndGetAddressOf()));

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(m_renderTexture.Get(), nullptr,
        m_shaderResourceView.ReleaseAndGetAddressOf()));
}

void RenderTarget::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);
}

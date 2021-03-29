#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(DX::DeviceResources & deviceResources, CD3D11_TEXTURE2D_DESC textureDesc)
{
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateTexture2D(&textureDesc, nullptr,
        m_renderTexture2D.GetAddressOf()));

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRenderTargetView(m_renderTexture2D.Get(), nullptr,
        m_renderTarget.ReleaseAndGetAddressOf()));

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(m_renderTexture2D.Get(), nullptr,
        m_shaderResourceView.ReleaseAndGetAddressOf()));
}

RenderTarget::RenderTarget(DX::DeviceResources & deviceResources, CD3D11_TEXTURE3D_DESC textureDesc)
{
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateTexture3D(&textureDesc, nullptr,
        m_renderTexture3D.GetAddressOf()));

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRenderTargetView(m_renderTexture3D.Get(), nullptr,
        m_renderTarget.ReleaseAndGetAddressOf()));

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(m_renderTexture3D.Get(), nullptr,
        m_shaderResourceView.ReleaseAndGetAddressOf()));
}

void RenderTarget::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, m_renderTarget.GetAddressOf(), nullptr);
}

#include "pch.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(DX::DeviceResources & deviceResources, CD3D11_TEXTURE2D_DESC textureDesc)
{
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateTexture2D(&textureDesc, nullptr,
        m_renderTexture2D.GetAddressOf()));

    // Setup the description of the render target view.
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRenderTargetView(m_renderTexture2D.Get(), &renderTargetViewDesc,
        m_renderTarget.ReleaseAndGetAddressOf()));

    // Setup the description of the shader resource view.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(m_renderTexture2D.Get(), &shaderResourceViewDesc,
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

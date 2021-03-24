#include "pch.h"
#include "BloomPostProcessEffect.h"

BloomPostProcessEffect::BloomPostProcessEffect(DX::DeviceResources & deviceResources)
{
    m_deviceContext = deviceResources.GetD3DDeviceContext();

    // get the backbuffer dimentions for the render textures
    auto width = deviceResources.GetBackBufferWidth();
    auto height = deviceResources.GetBackBufferHeight();
    auto backBufferFormat = deviceResources.GetBackBufferFormat();

    m_spriteBatch = std::make_unique<SpriteBatch>(deviceResources.GetD3DDeviceContext());

    // create the pixel shaders
    auto blob = DX::ReadData(L"BloomExtract.cso");
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_bloomExtractPS.ReleaseAndGetAddressOf()));

    blob = DX::ReadData(L"BloomCombine.cso");
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_bloomCombinePS.ReleaseAndGetAddressOf()));

    blob = DX::ReadData(L"GaussianBlur.cso");
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_gaussianBlurPS.ReleaseAndGetAddressOf()));

    // Create the pixel shader constant buffers
    {
        CD3D11_BUFFER_DESC cbDesc(sizeof(BloomConstantBuffer::BloomBufferType),
            D3D11_BIND_CONSTANT_BUFFER);
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &BloomConstantBuffer::bloomSetting;
        DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&cbDesc, &initData,
            m_bloomParams.ReleaseAndGetAddressOf()));
    }
    {
        CD3D11_BUFFER_DESC cbDesc(sizeof(GaussianBlurConstantBuffer::BlurBufferType),
            D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&cbDesc, nullptr,
            m_blurParamsWidth.ReleaseAndGetAddressOf()));
        DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&cbDesc, nullptr,
            m_blurParamsHeight.ReleaseAndGetAddressOf()));
    }

    GaussianBlurConstantBuffer::BlurBufferType blurData;
    blurData.SetBlurEffectParameters(1.f / (width / 2), 0,
        BloomConstantBuffer::bloomSetting);
    m_deviceContext->UpdateSubresource(m_blurParamsWidth.Get(), 0, nullptr,
        &blurData, sizeof(GaussianBlurConstantBuffer::BlurBufferType), 0);

    blurData.SetBlurEffectParameters(0, 1.f / (height / 2),
        BloomConstantBuffer::bloomSetting);
    m_deviceContext->UpdateSubresource(m_blurParamsHeight.Get(), 0, nullptr,
        &blurData, sizeof(GaussianBlurConstantBuffer::BlurBufferType), 0);

    // Render target for the orignal scene
    CD3D11_TEXTURE2D_DESC sceneDesc(backBufferFormat, width, height,
        1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateTexture2D(&sceneDesc, nullptr,
        m_originalSceneTexture.GetAddressOf()));
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRenderTargetView(m_originalSceneTexture.Get(), nullptr,
        m_sceneRenderTarget.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(m_originalSceneTexture.Get(), nullptr,
        m_sceneShaderResourceView.ReleaseAndGetAddressOf()));

    // Render targets for the horizontal and vertical blurring
    CD3D11_TEXTURE2D_DESC rtDesc(backBufferFormat, width / 2, height / 2,
        1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
    Microsoft::WRL::ComPtr<ID3D11Texture2D> blurRenderTexture1;
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateTexture2D(&rtDesc, nullptr,
        blurRenderTexture1.GetAddressOf()));
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRenderTargetView(blurRenderTexture1.Get(), nullptr,
        m_bloomRenderTarget1.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(blurRenderTexture1.Get(), nullptr,
        m_bloomShaderResourceView1.ReleaseAndGetAddressOf()));

    Microsoft::WRL::ComPtr<ID3D11Texture2D> blurRenderTexture2;
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateTexture2D(&rtDesc, nullptr,
        blurRenderTexture2.GetAddressOf()));
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateRenderTargetView(blurRenderTexture2.Get(), nullptr,
        m_bloomRenderTarget2.ReleaseAndGetAddressOf()));
    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(blurRenderTexture2.Get(), nullptr,
        m_bloomShaderResourceView2.ReleaseAndGetAddressOf()));

    m_bloomRect.left = 0;
    m_bloomRect.top = 0;
    m_bloomRect.right = width / 2;
    m_bloomRect.bottom = height / 2;

    m_fullscreenRect.left = 0;
    m_fullscreenRect.top = 0;
    m_fullscreenRect.right = width;
    m_fullscreenRect.bottom = height;
}

void BloomPostProcessEffect::Bind(DX::DeviceResources & deviceResources) noexcept
{
    ID3D11ShaderResourceView* null[] = { nullptr, nullptr };

    // Apply the brightness threshold
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, m_bloomRenderTarget1.GetAddressOf(), nullptr);

    m_spriteBatch->Begin(SpriteSortMode_Immediate,
        nullptr, nullptr, nullptr, nullptr,
        [=]() {
        deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
        deviceResources.GetD3DDeviceContext()->PSSetShader(m_bloomExtractPS.Get(), nullptr, 0);
    });

    m_spriteBatch->Draw(m_sceneShaderResourceView.Get(), m_bloomRect);
    m_spriteBatch->End();

    // Horizontal blur
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, m_bloomRenderTarget2.GetAddressOf(), nullptr);

    m_spriteBatch->Begin(SpriteSortMode_Immediate,
        nullptr, nullptr, nullptr, nullptr,
        [=]() {
        deviceResources.GetD3DDeviceContext()->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
        deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1,
            m_blurParamsWidth.GetAddressOf());
    });

    m_spriteBatch->Draw(m_bloomShaderResourceView1.Get(), m_bloomRect);
    m_spriteBatch->End();

    deviceResources.GetD3DDeviceContext()->PSSetShaderResources(0, 2, null);

    // Vertical blur
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, m_bloomRenderTarget1.GetAddressOf(), nullptr);

    m_spriteBatch->Begin(SpriteSortMode_Immediate,
        nullptr, nullptr, nullptr, nullptr,
        [=]() {
        deviceResources.GetD3DDeviceContext()->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
        deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1,
            m_blurParamsHeight.GetAddressOf());
    });

    m_spriteBatch->Draw(m_bloomShaderResourceView2.Get(), m_bloomRect);
    m_spriteBatch->End();

    // Combine
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1, deviceResources.GetRenderTargetViewAddress(), nullptr); // main render target view
    m_spriteBatch->Begin(SpriteSortMode_Immediate,
        nullptr, nullptr, nullptr, nullptr,
        [=]() {
        deviceResources.GetD3DDeviceContext()->PSSetShader(m_bloomCombinePS.Get(), nullptr, 0);
        deviceResources.GetD3DDeviceContext()->PSSetShaderResources(1, 1, m_bloomShaderResourceView1.GetAddressOf());
        deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
    });
    m_spriteBatch->Draw(m_sceneShaderResourceView.Get(), m_fullscreenRect);
    m_spriteBatch->End();

    deviceResources.GetD3DDeviceContext()->PSSetShaderResources(0, 2, null);
}

void BloomPostProcessEffect::SetSceneRenderTarget(DX::DeviceResources & deviceResources)
{
    deviceResources.GetD3DDeviceContext()->OMSetRenderTargets(1,  m_sceneRenderTarget.GetAddressOf(), deviceResources.GetDepthStencilView());
}


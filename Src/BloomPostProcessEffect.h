#pragma once
#include "pch.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "InputLayout.h"
#include "Bindable.h"
#include "RenderTexture.h"
#include "BloomConstantBuffer.h"
#include "GaussianBlurConstantBuffer.h"
#include "Texture.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "Sampler.h"

class BloomPostProcessEffect : public Bindable
{
public:
	BloomPostProcessEffect(DX::DeviceResources& deviceResources);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
    void SetSceneRenderTarget(DX::DeviceResources & deviceResources);
private:
    ID3D11DeviceContext* m_deviceContext;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_originalSceneTexture;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_sceneShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_sceneRenderTarget;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bloomShaderResourceView1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_bloomRenderTarget1;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_bloomShaderResourceView2;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_bloomRenderTarget2;

    RECT m_bloomRect;
    RECT m_fullscreenRect;

    std::unique_ptr<BloomConstantBuffer> m_bloomConstantBuffer;
    std::unique_ptr<PixelShader> m_bloomExtractPixelShader;
    std::unique_ptr<PixelShader> m_bloomCombinePixelShader;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomExtractPS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomCombinePS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianBlurPS;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_bloomParams;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_blurParamsWidth;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_blurParamsHeight;

    std::unique_ptr<GaussianBlurConstantBuffer> m_blurWidthConstantBuffer;
    std::unique_ptr<GaussianBlurConstantBuffer> m_blurHeightConstantBuffer;
    std::unique_ptr<PixelShader> m_gaussianBlurPixelShader;

    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
};


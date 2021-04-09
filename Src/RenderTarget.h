#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class RenderTarget :
	public Bindable
{
public:
	RenderTarget(DX::DeviceResources& deviceResources, CD3D11_TEXTURE2D_DESC textureDesc);
    RenderTarget(DX::DeviceResources& deviceResources, CD3D11_TEXTURE3D_DESC textureDesc);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
    ID3D11ShaderResourceView** GetShaderResourceViewAddress() { return m_shaderResourceView.GetAddressOf(); }
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTarget;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTexture2D;
    Microsoft::WRL::ComPtr<ID3D11Texture3D> m_renderTexture3D;
};


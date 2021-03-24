#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class RenderTarget :
	public Bindable
{
public:
	RenderTarget(DX::DeviceResources& deviceResources);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTarget;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_renderTexture;
};


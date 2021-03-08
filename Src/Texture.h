#pragma once
#include "Bindable.h"
#include "DeviceResources.h"


class Texture :
	public Bindable
{
public:
	Texture(DX::DeviceResources& deviceResource, const wchar_t* fileName);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
    ID3D11ShaderResourceView* Get() const { return m_textureSrv.Get(); }
    DirectX::SimpleMath::Vector2 GetOrigin() const;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureSrv;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    DirectX::SimpleMath::Vector2 m_origin;
    DirectX::SimpleMath::Vector2 m_position;
};


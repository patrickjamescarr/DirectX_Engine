#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class CubeTexture : public Bindable 
{
public:
	CubeTexture(DX::DeviceResources& deviceResources, const wchar_t* fileName);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
};


#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class PixelShader : public Bindable
{
public:
	PixelShader(DX::DeviceResources & deviceResources, const  WCHAR * fileName);
	void Bind(DX::DeviceResources & deviceResources) noexcept override;
    void Set();
protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    DX::DeviceResources & m_deviceResources;
};


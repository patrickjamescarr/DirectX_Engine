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
	ID3D11DepthStencilView* m_depthStencilView;
};


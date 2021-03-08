#pragma once
#include "DeviceResources.h"
#include "Bindable.h"

class DepthStencil :
	public Bindable
{
public:
	DepthStencil(DX::DeviceResources& deviceResources);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
	ID3D11DepthStencilView* GetPtr() const
	{
		return m_depthStencilView.Get();
	}
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
};


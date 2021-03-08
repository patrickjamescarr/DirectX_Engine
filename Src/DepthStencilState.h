#pragma once
#include "Bindable.h"
#include "DeviceResources.h"

class DepthStencilState :
	public Bindable
{
public:
	enum State {
		default,
		depthOff
	};
	DepthStencilState(DX::DeviceResources& deviceResources, State state);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_state;
};


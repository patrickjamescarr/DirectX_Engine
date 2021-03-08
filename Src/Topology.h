#pragma once
#include "Bindable.h"

class Topology : public Bindable 
{
public:
	Topology(DX::DeviceResources& deviceResources, D3D11_PRIMITIVE_TOPOLOGY topology);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY topology;
};


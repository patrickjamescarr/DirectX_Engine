#pragma once
#include "DeviceResources.h"

class Bindable
{
public:
	virtual void Bind(DX::DeviceResources& deviceResources) noexcept = 0;
	virtual ~Bindable() = default;
};


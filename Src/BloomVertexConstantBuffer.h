#pragma once
#include "Bindable.h"
#include "DeviceResources.h"
#include "VertexConstantBuffer.h"

class BloomVertexConstantBuffer :
    public Bindable
{
public:
    BloomVertexConstantBuffer(DX::DeviceResources& deviceResources);
};


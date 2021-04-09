#pragma once
#include "ConstantBuffer.h"

template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T>
{
	using ConstantBuffer<T>::m_constantBuffer;
public:
	VertexConstantBuffer(DX::DeviceResources& deviceResources, UINT slot = 0) 
        : ConstantBuffer(deviceResources), m_slot(slot)
	{
	}
	void Bind(DX::DeviceResources& deviceResources) noexcept override
	{
		deviceResources.GetD3DDeviceContext()->VSSetConstantBuffers(m_slot, 1, m_constantBuffer.GetAddressOf());
	}
private:
    UINT m_slot;
};


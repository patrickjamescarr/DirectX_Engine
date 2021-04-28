#pragma once
#include "ConstantBuffer.h"

template <typename T>
class PixelConstantBuffer : public ConstantBuffer<T>
{
	using ConstantBuffer<T>::m_constantBuffer;
public:
	PixelConstantBuffer(DX::DeviceResources& deviceResources, UINT slot = 0) : ConstantBuffer(deviceResources), m_slot(slot)
	{}

	PixelConstantBuffer(DX::DeviceResources& deviceResources, const T& consts, UINT slot = 0) : ConstantBuffer(deviceResources, consts), m_slot(slot)
	{}

	void Bind(DX::DeviceResources& deviceResources) noexcept override
	{
		deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(m_slot, 1, m_constantBuffer.GetAddressOf());
	}
private:
    UINT m_slot;
};


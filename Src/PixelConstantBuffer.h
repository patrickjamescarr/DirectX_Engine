#pragma once
#include "ConstantBuffer.h"

template <typename T>
class PixelConstantBuffer : public ConstantBuffer<T>
{
	using ConstantBuffer<T>::m_constantBuffer;
public:
	PixelConstantBuffer(DX::DeviceResources& deviceResources) : ConstantBuffer(deviceResources)
	{}

	PixelConstantBuffer(DX::DeviceResources& deviceResources, const T& consts) : ConstantBuffer(deviceResources, consts)
	{}

	void Bind(DX::DeviceResources& deviceResources) noexcept override
	{
		deviceResources.GetD3DDeviceContext()->PSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
	}
};


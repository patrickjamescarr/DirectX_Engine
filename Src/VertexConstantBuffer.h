#pragma once
#include "ConstantBuffer.h"

template<typename T>
class VertexConstantBuffer : public ConstantBuffer<T>
{
	using ConstantBuffer<T>::m_constantBuffer;
public:
	VertexConstantBuffer(DX::DeviceResources& deviceResources) : ConstantBuffer(deviceResources)
	{
	}
	void Bind(DX::DeviceResources& deviceResources) noexcept override
	{
		deviceResources.GetD3DDeviceContext()->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
	}
};


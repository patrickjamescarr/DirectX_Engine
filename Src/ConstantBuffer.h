#pragma once
#include "Bindable.h"

template <class T>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(DX::DeviceResources& deviceResources)
	{
		// Setup the description of the dynamic constant buffer
		D3D11_BUFFER_DESC	constantBufferDesc;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.ByteWidth = sizeof(T);
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		// Create the dynamic constant buffer pointer
		DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&constantBufferDesc, NULL, &m_constantBuffer));

		bufferCreated = true;
	}

	ConstantBuffer(DX::DeviceResources& deviceResources, const T& consts)
	{
		// Setup the description of the dynamic constant buffer
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;

		// Create the dynamic constant buffer pointer
		DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&cbd, &csd, &m_constantBuffer));

		bufferCreated = true;
	}

	void Update(DX::DeviceResources& deviceResources, const T& typeData)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;

		assert(bufferCreated);

		DX::ThrowIfFailed(deviceResources.GetD3DDeviceContext()->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

		memcpy(mappedResource.pData, &typeData, sizeof(typeData));

		deviceResources.GetD3DDeviceContext()->Unmap(m_constantBuffer.Get(), 0);
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
private:
	bool bufferCreated = false;
};


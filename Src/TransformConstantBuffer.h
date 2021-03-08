#pragma once
#include "Bindable.h"
#include "GameObject.h"
#include "VertexConstantBuffer.h"

class TransformConstantBuffer : public Bindable
{
private:
	//standard matrix buffer supplied to all shaders
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};
public:
	TransformConstantBuffer(DX::DeviceResources& deviceResources, const GameObject& parent);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
private:
	std::unique_ptr<VertexConstantBuffer<MatrixBufferType>> m_vertexConstantBuffer;
	const GameObject& m_parent;
};


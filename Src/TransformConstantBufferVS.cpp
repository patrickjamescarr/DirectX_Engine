#include "pch.h"
#include "TransformConstantBufferVS.h"

TransformConstantBufferVS::TransformConstantBufferVS(DX::DeviceResources & deviceResources, const GameObject & parent, UINT slot)
	: m_parent(parent)
{
	if (!m_vertexConstantBuffer)
	{
		m_vertexConstantBuffer = std::make_unique<VertexConstantBuffer<MatrixBufferType>>(deviceResources, slot);
	}
}

void TransformConstantBufferVS::Bind(DX::DeviceResources & deviceResources) noexcept
{
	const MatrixBufferType matrixBuffer =
	{
		DirectX::XMMatrixTranspose(m_parent.GetTransform()),
		DirectX::XMMatrixTranspose(deviceResources.GetView()),
		DirectX::XMMatrixTranspose(deviceResources.GetProjection())
	};

	m_vertexConstantBuffer->Update(deviceResources, matrixBuffer);
	m_vertexConstantBuffer->Bind(deviceResources);
}

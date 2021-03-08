#include "pch.h"
#include "SkyBox.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Topology.h"
#include "CubeTexture.h"
#include "DepthStencil.h"
#include "DepthStencilState.h"
#include "RasterizerState.h"
#include "InputLayout.h"
#include "Sampler.h"
#include "SkyboxConstantBuffer.h"
#include "RenderTarget.h"

SkyBox::SkyBox(DX::DeviceResources & deviceResources)
{
	// create the skybox texture
	AddBind(std::make_unique<CubeTexture>(deviceResources, L"SkyBox\\space_skybox.dds"));

	// create a depth stencil state that turns off the depth buffer
	AddBind(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::depthOff));

	// create a rasterizer state to turn off back face culling
	AddBind(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_NONE));

	// setup the constant buffer for the skybox vertex shader
	AddBind(std::make_unique<SkyboxConstantBuffer>(deviceResources));

	// standard triangle list topology
	AddBind(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	// create the skybox vertex shader
	auto vertexShader = std::make_unique<VertexShader>(deviceResources, L"skybox_vs.cso");
	auto vertexShaderByteCode = vertexShader->GetBytecode();
	AddBind(std::move(vertexShader));

	// create a unit cube for the skybox
	std::vector<DirectX::VertexPositionNormalTexture> cubeVertices;
	std::vector<uint16_t> cubeIndices;
	DirectX::GeometricPrimitive::CreateBox(cubeVertices, cubeIndices, DirectX::SimpleMath::Vector3(1, 1, 1), false);

	// Create the vertex buffer
	AddBind(std::make_unique<VertexBuffer>(deviceResources, cubeVertices));

	// Create the index buffer
	AddIndexBuffer(std::make_unique<IndexBuffer>(deviceResources, cubeIndices));

	// Create the vertex input layout description.
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	AddBind(std::make_unique<InputLayout>(deviceResources, layout, vertexShaderByteCode));

	AddBind(std::make_unique<PixelShader>(deviceResources, L"skybox_ps.cso"));
	AddBind(std::make_unique<Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_WRAP));
}

void SkyBox::Draw(DX::DeviceResources & deviceResources) const
{
	GameObject::Draw(deviceResources);
}

DirectX::SimpleMath::Matrix SkyBox::GetTransform() const noexcept
{
	return DirectX::SimpleMath::Matrix();
}

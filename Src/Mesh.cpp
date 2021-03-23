#include "pch.h"
#include "Mesh.h"
#include "TransformConstantBuffer.h"
#include "LightConstantBuffer.h"
#include "Texture.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "Topology.h"
#include "DepthStencil.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Shader.h"
#include "InputLayout.h"
#include "Sampler.h"


Mesh::Mesh(DirectX::SimpleMath::Matrix transform)
    : m_transform(transform)
{
}

Mesh::Mesh(
    DX::DeviceResources & deviceResources, 
    std::vector<std::unique_ptr<Bindable>> bindables, 
    Light* light, 
    const wchar_t* textureFileName, 
    DirectX::SimpleMath::Matrix transform,
    D3D_PRIMITIVE_TOPOLOGY topology
)
    : m_transform(transform)
{
    AddDefaultBindables(deviceResources, light, textureFileName, topology);

    AddBindables(bindables);
}

Mesh::Mesh(DX::DeviceResources & deviceResources, Light * light, DirectX::SimpleMath::Matrix transform, const wchar_t * textureFileName, const wchar_t * vertexShaderFileName, const wchar_t * pixelShaderFileName)
{
    AddDefaultBindables(deviceResources, light, textureFileName, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create the vertex shader
    auto vertexShader = std::make_unique<VertexShader>(deviceResources, vertexShaderFileName);
    auto vertexShaderByteCode = vertexShader->GetBytecode();
    AddBind(std::move(vertexShader));

    // Create the pixel shader
    AddBind(std::make_unique<PixelShader>(deviceResources, pixelShaderFileName));

    AddBind(std::make_unique<Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_WRAP));

    // Create the vertex input layout description.
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // Create the input layout
    AddBind(std::make_unique<InputLayout>(deviceResources, layout, vertexShaderByteCode));

    // Create the rasterizer state
    AddBind(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK)); //D3D11_FILL_WIREFRAME
}

void Mesh::AddDefaultBindables(DX::DeviceResources & deviceResources, Light * &light, const wchar_t * &textureFileName, D3D_PRIMITIVE_TOPOLOGY topology)
{
    AddBind(std::make_unique<Topology>(deviceResources, topology));

    AddBind(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));

    AddBind(std::make_unique<TransformConstantBuffer>(deviceResources, *this));

    if (light)
    {
        AddBind(std::make_unique<LightConstantBuffer>(deviceResources, light));
    }

    if (textureFileName)
    {
        AddBind(std::make_unique<Texture>(deviceResources, textureFileName));
    }
}

Mesh::Mesh(DX::DeviceResources & deviceResources, Light * light, const wchar_t * textureFileName, DirectX::SimpleMath::Matrix transform)
    : m_transform(transform)
{
    AddDefaultBindables(deviceResources, light, textureFileName, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::AddBindables(std::vector<std::unique_ptr<Bindable>>& bindables)
{
	for (auto& bind : bindables)
	{
		if (auto indexBuffer = dynamic_cast<IndexBuffer*>(bind.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{ indexBuffer });
			bind.release();
		}
		else
		{
			AddBind(std::move(bind));
		}
	}
}

void Mesh::Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const
{
    m_accumulatedTransform = accumulatedTransform;

	GameObject::Draw(deviceResources);
}

DirectX::SimpleMath::Matrix Mesh::GetTransform() const noexcept
{
	return m_transform * m_accumulatedTransform;
}

void Mesh::SetTransform(DirectX::SimpleMath::Matrix transform) noexcept
{
    m_transform = transform;
}
#include "pch.h"
#include "Mesh.h"
#include "TransformConstantBuffer.h"
#include "LightConstantBuffer.h"
#include "Texture.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "Topology.h"
#include "DepthStencil.h"


Mesh::Mesh(DirectX::SimpleMath::Matrix transform)
    : m_transform(transform)
{
}

Mesh::Mesh(DX::DeviceResources & deviceResources, std::vector<std::unique_ptr<Bindable>> bindables, Light* light, const wchar_t* textureFileName, DirectX::SimpleMath::Matrix transform)
    : m_transform(transform)
{
    AddDefaultBindables(deviceResources, light, textureFileName);

    AddBindables(bindables);
}

void Mesh::AddDefaultBindables(DX::DeviceResources & deviceResources, Light * &light, const wchar_t * &textureFileName)
{
    AddBind(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

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
    AddDefaultBindables(deviceResources, light, textureFileName);
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
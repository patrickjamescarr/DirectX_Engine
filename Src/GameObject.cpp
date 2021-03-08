#include "pch.h"
#include "GameObject.h"

GameObject::~GameObject()
{
}

/// <summary>
/// Draw this object
/// </summary>
void GameObject::Draw(DX::DeviceResources & deviceResources) const
{
	// bind all of this obejcts bindables to the pipeline
	for (auto& b : binds)
	{
		b->Bind(deviceResources);
	}

	// draw the object
	deviceResources.GetD3DDeviceContext()->DrawIndexed(m_indexBuffer->GetCount(), 0, 0);
}

/// <summary>
/// Add a bindable pipeline object
/// </summary>
void GameObject::AddBind(std::unique_ptr<Bindable> bind)
{
	binds.push_back(std::move(bind));
}

/// <summary>
/// Add an index buffer to this object
/// </summary>
void GameObject::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer)
{
	m_indexBuffer = indexBuffer.get();
	binds.push_back(std::move(indexBuffer));
}

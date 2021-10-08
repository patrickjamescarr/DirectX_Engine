#include "pch.h"
#include "SceneNode.h"

SceneNode::SceneNode(std::vector<Mesh*> meshes, const DirectX::SimpleMath::Matrix & transform) 
	: m_meshes(std::move(meshes)), m_transform(transform)
{
}

SceneNode::SceneNode(const DirectX::SimpleMath::Matrix & transform)
{
}

/// <summary>
/// Draw the meshes in this node, and all the child nodes
/// </summary>
void SceneNode::Draw(DX::DeviceResources& deviceResources, const DirectX::SimpleMath::Matrix& accumulatedTransform)
{
	// multiply the accumulated transform passed in with the current transform to create an updated transfom
	const auto newTransform = m_transform * accumulatedTransform;

	// draw all the mesh obejcts, passing in the updated transform
	for (const auto mesh : m_meshes)
	{
        mesh->Update();
		mesh->Draw(deviceResources, newTransform);
	}

	// draw the child nodes, passing in the udpated transform
	for (const auto& child : m_children)
	{
		child->Draw(deviceResources, newTransform);
	}
}

void SceneNode::AddMesh(Mesh* mesh)
{
    m_meshes.push_back(std::move(mesh));
}

/// <summary>
/// Adds a child node
/// </summary>
void SceneNode::AddChild(std::unique_ptr<SceneNode> child)
{
	m_children.push_back(std::move(child));
}
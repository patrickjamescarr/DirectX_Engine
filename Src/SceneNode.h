#pragma once

#include "ModelClass.h"
#include "Mesh.h"
#include "Light.h"

class SceneNode
{
	friend class MainScene;
public:
	SceneNode(std::vector<Mesh*> meshes, const DirectX::SimpleMath::Matrix & transform);
    SceneNode(const DirectX::SimpleMath::Matrix & transform);
    virtual void Draw(DX::DeviceResources& deviceResources, const DirectX::SimpleMath::Matrix& accumulatedTransform);
protected:
    void AddMesh(Mesh* mesh);
    void AddChild(std::unique_ptr<SceneNode> node);
protected:
    DirectX::SimpleMath::Matrix m_transform;
private:
	std::vector<std::unique_ptr<SceneNode>> m_children;
	std::vector<Mesh*> m_meshes;
};


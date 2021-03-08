#pragma once

#include "DeviceResources.h"
#include "Mesh.h"
#include "SceneNode.h"
#include "ModelLoader.h"
#include "BloomPostProcessEffect.h"
#include "SkyBox.h"
#include "LensFlareEffect.h"
#include "PlayerCamera.h"
#include "CollisionDetection.h"
#include "StepTimer.h"
#include "Collision.h"
#include "ViewingFrustum.h"

class MainScene
{
public:
	MainScene();
	void Initialise(DX::DeviceResources& deviceResources, PlayerCamera* playerCamera, ViewingFrustum* frustum);
	void Draw(DX::DeviceResources& deviceResources, const DX::StepTimer& timer) const;
private:

	//buffer for information of a single light
	struct LightBufferType
	{
		DirectX::SimpleMath::Vector4 ambient;
		DirectX::SimpleMath::Vector4 diffuse;
		DirectX::SimpleMath::Vector3 position;
		float padding;
	};

	static std::unique_ptr<Mesh> ParseMesh(
        DX::DeviceResources& deviceResources, 
        const MeshObject& modelMesh, 
        Light * sceneLight, 
        DirectX::SimpleMath::Matrix transform,
        const wchar_t * textureFileName
    );
	std::unique_ptr<SceneNode> ParseNode();

private:
    // object containers
	std::unique_ptr<SceneNode> m_rootNode;
    std::unique_ptr<SceneNode> m_terrainNode;
	std::vector<std::unique_ptr<Mesh>> m_meshes;

    ModelLoader m_modelLoader;

    DirectX::SimpleMath::Matrix m_terrainTransform;

    int m_terrainWidth = 128;
    int m_terrainHeight = 128;

    PlayerCamera* m_playerCamera;

    // Light source
	std::unique_ptr<Light> m_light;
    std::unique_ptr<Light> m_sunLight;

    // Skybox
    std::unique_ptr<SkyBox> m_skyBox;

    // Post processing
    std::unique_ptr<BloomPostProcessEffect> m_bloom;
    std::unique_ptr<LensFlareEffect> m_lensFlare;

    // Scene transformation matrix
    DirectX::XMMATRIX m_transform = DirectX::SimpleMath::Matrix::Identity;


    CollisionDetection m_collisionDetection;


    std::unique_ptr<Collision> m_collisionDetector;
};


#pragma once
#include "Mesh.h"
#include "ModelLoader.h"
#include "SimplexNoise.h"
#include "VertexBuffer.h"
#include "fBm.h"
#include "CollisionDetection.h"
#include "PlayerCamera.h"
#include "ViewingFrustum.h"
#include "QuadTree.h"

class Terrain : public Mesh
{
public:
    enum TerrainType
    {
        Flat = 0,
        RandomHeight = 1,
        Fault = 2,
        Smooth = 3,
        Noise = 4,
        FBMTerrain = 5
    };

    enum NoiseType
    {
        Perlin = 0,
        FBMNoise = 1
    };

    Terrain(
        DX::DeviceResources& deviceResources, 
        Light* light, 
        const wchar_t* textureFileName, 
        DirectX::SimpleMath::Matrix transform, 
        int terrainWidth, 
        int terrainHeight,
        float scale,
        PlayerCamera* playerCamera,
        ViewingFrustum* frustum
    );

    void CalculateHeightMapTextureCoordinates();
    ~Terrain();
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const noexcept override; // override;
    virtual void Update();

    const MeshObject& GetMesh() { return m_terrain; }

private:
    bool CalculateNormals();
    void DisplayGUI() noexcept;

    void GenerateHeightMap();
    void FlattenTerrain();
    float GenerateRandomHeightValue();
    void ApplySmoothing();
    void ApplyFaulting();
    void GenerateNoise(NoiseType noiseType);
    void CreateFaultLine(float stepSize);
    void CreateFmbFaultLine(float stepSize);
    void Generate();

    bool DetectCameraCollision() const;

private:

    std::vector<uint32_t> m_indices;

    int m_terrainWidth, m_terrainHeight;
    ModelLoader m_modelLoader;
    HeightMapType* m_heightMap;
    MeshObject m_terrain;
    SimplexNoise m_noise;
    FBM fbm;
    CollisionDetection m_collider;

    DX::DeviceResources& m_deviceResources;
    std::shared_ptr<VertexBuffer<DirectX::VertexPositionNormalTexture>> m_vertexBuffer;

    std::unique_ptr<QuadTree> m_quadTree;

    PlayerCamera* m_camera;

    ViewingFrustum* m_frustum;

    float m_scale;

    float m_randomHeightValue = 3.0f;
    int m_selectedTerrain;
    float m_faultStepSize = 1.0f;
    bool m_faultUp = false;

    // fbm
    float fbm_amp = 0.5f;
    float fbm_val = 0.0f;
    int fbm_octaves = 6;
    float fbm_lacunarity = 2.0f;
    float fbm_gain = 0.5f;
};


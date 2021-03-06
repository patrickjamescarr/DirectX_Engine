#pragma once
#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "DensityFunction.h"
#include "SimplexFunction.h"
#include "MarchingCubes.h"
#include "BrownianFunction.h"
#include "TerrainDensityFunction.h"
#include "Camera.h"

class MarchingCubesMesh :
    public Mesh
{
private:
    enum FunctionType {
        Simplex = 0,
        Brownian = 1,
        Terrain = 2
    };
public:
    MarchingCubesMesh(
        DX::DeviceResources& deviceResources,
        Light * sceneLight,
        DirectX::SimpleMath::Matrix transform,
        const wchar_t * textureFileName,
        const wchar_t * vertexShaderFileName,
        const wchar_t * pixelShaderFileName,
        Camera * camera
        
    );

    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const override;

public:
    void Update();
    void ShowFBMControls();
private:
    VertexBuffer<DirectX::VertexPositionNormalTexture>* m_vertexBuffer;
    IndexBuffer* m_indexBuffer;

    std::unique_ptr<MarchingCubes> m_marchingCubes;
    DensityFunction* m_function;
    SimplexFunction m_simplexFunction;
    BrownianFunction m_brownianFunction;
    std::unique_ptr<TerrainDensityFunction> m_terrainFunction;

    DirectX::SimpleMath::Vector3 m_cubeDimentions;

    DX::DeviceResources& m_deviceResources;

    int m_functionType = Simplex;

    float m_fogEnd = 10.0f;

    int m_width = 10;
    int m_depth = 10;
    int m_height = 10;

};


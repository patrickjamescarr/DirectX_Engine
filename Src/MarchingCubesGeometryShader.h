#pragma once
#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Bindable.h"
#include "Sampler.h"
#include "Camera.h"
#include "OnScreenQuad.h"
#include "ViewingFrustum.h"
#include "HeightMapTerrain.h"

class MarchingCubesGeometryShader :
    public Mesh
{
public:
    MarchingCubesGeometryShader(
        DX::DeviceResources& deviceResources,
        Light * sceneLight,
        Camera* activeCamera,
        ViewingFrustum* viewingFrustum,
        DirectX::SimpleMath::Matrix transform,
        int xPos,
        int yPos,
        int zPos,
        const float * isoLevel,
        const float * fogEnd,
        const int &dimention,
        float scale
        );
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const noexcept override;
    void RenderCubeTerrain(DX::DeviceResources & deviceResources, const DirectX::XMMATRIX &accumulatedTransform) const;
    void BuildDensityVolumeRenderPass(DX::DeviceResources & deviceResources) const;
    void GenerateVerticesRenderPass(DX::DeviceResources & deviceResources) const;
    virtual void Update();
private:
    std::unique_ptr<OnScreenQuad> m_quad;

    std::unique_ptr<PixelShader> m_build_densities_PS;
    std::unique_ptr<VertexShader> m_build_densities_VS;

    RenderTarget* m_buildDensitiesRT;
    std::vector < std::unique_ptr<Bindable>> m_densityVolumeRenderPass;

    VertexBuffer<DirectX::VertexPosition>* m_densityVertexBuffer;

    RenderTarget* m_generateVertsRT;
    std::vector < std::unique_ptr<Bindable>> m_generateVertsRenderPass;
    VertexBuffer<DirectX::VertexPositionTexture>* m_generateVertsVertexBuffer;
    UINT m_generateVertexCount;

    std::unique_ptr<Sampler> m_gsSampler;

    ViewingFrustum* m_frustum;

    DirectX::SimpleMath::Matrix m_viewingFrustumTransform;

    DirectX::SimpleMath::Vector3 m_position;

    Camera* m_playerCamera;

    std::unique_ptr<HeightMapTerrain> m_floorTerrain;

    const float * m_isoLevel;
    const float * m_fogEnd;
    float m_scale;
    const int &m_dimention;
    int m_yPos;
    float m_fogStart = 0.0f;
};


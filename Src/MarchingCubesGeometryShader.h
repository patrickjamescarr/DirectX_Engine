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

class MarchingCubesGeometryShader :
    public Mesh
{
public:
    MarchingCubesGeometryShader(
        DX::DeviceResources& deviceResources,
        Light * sceneLight,
        Camera* activeCamera,
        DirectX::SimpleMath::Matrix transform,
        int xPos,
        int zPos
        );
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const noexcept override;
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

    float m_isoLevel = 0.0f;
    float m_scale = 0.03f;
    int m_dimention = 65;
};


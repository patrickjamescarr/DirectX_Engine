#pragma once
#include "Mesh.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "RenderTarget.h"
#include "Texture.h"

class MarchingCubesGeometryShader :
    public Mesh
{
public:
    MarchingCubesGeometryShader(
        DX::DeviceResources& deviceResources,
        Light * sceneLight,
        DirectX::SimpleMath::Matrix transform
        );
    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const noexcept override;
private:
    const wchar_t * m_textureFileName = L"Textures//sun2.dds";
    const wchar_t * m_vertexShaderFileName = L"test_vs.cso";
    const wchar_t * m_pixelShaderFileName = L"test_ps.cso";

    VertexBuffer* m_vertexBuffer;
    IndexBuffer* m_indexBuffer;

    std::unique_ptr<RenderTarget> m_build_densities_RT;
    std::unique_ptr<PixelShader> m_build_densities_PS;
    std::unique_ptr<VertexShader> m_build_densities_VS;
    std::unique_ptr<GeometryShader> m_build_densities_GS;
};


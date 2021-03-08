#pragma once
#include "VertexShader.h"
#include "PixelShader.h"
#include "InputLayout.h"
#include "Sampler.h"

class TerrainShader
{
private:
    struct MatrixBufferType
    {
        DirectX::SimpleMath::Matrix world;
        DirectX::SimpleMath::Matrix view;
        DirectX::SimpleMath::Matrix projection;
    };

    struct LightBufferType
    {
        DirectX::SimpleMath::Vector4 ambientColor;
        DirectX::SimpleMath::Vector4 diffuseColor;
        DirectX::SimpleMath::Vector4 lightDirection;
        float padding;
    };
public:
    TerrainShader();
    ~TerrainShader() {};

    bool Initialize(DX::DeviceResources& deviceResources, WCHAR* vsFilename, WCHAR* psFilename);
    void Shutdown();
    bool Render(
        DX::DeviceResources& deviceResources,
        int indexCount,
        DirectX::SimpleMath::Matrix worldMatrix,
        DirectX::SimpleMath::Matrix viewMatrix,
        DirectX::SimpleMath::Matrix projectionMatrix,
        DirectX::SimpleMath::Vector4 ambientColor,
        DirectX::SimpleMath::Vector4 diffuseColor,
        DirectX::SimpleMath::Vector3 lightDirection,
        ID3D11ShaderResourceView* texture
    );
private:
    void ShutdownShader();
    void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

    bool SetShaderParameters(
        DX::DeviceResources& deviceResources,
        DirectX::SimpleMath::Matrix worldMatrix,
        DirectX::SimpleMath::Matrix viewMatrix,
        DirectX::SimpleMath::Matrix projectionMatrix,
        DirectX::SimpleMath::Vector4 ambientColor,
        DirectX::SimpleMath::Vector4 diffuseColor,
        DirectX::SimpleMath::Vector3 lightDirection,
        ID3D11ShaderResourceView* texture
    );

    void Bind(DX::DeviceResources& deviceResources);
    void Draw(DX::DeviceResources& deviceResources, int indexCount);
private:
private:
    std::unique_ptr<VertexShader> m_vertexShader;
    std::unique_ptr<PixelShader> m_pixelShader;
    std::unique_ptr<InputLayout> m_inputLayout;
//    std::unique_ptr<InputLayout> m_inputLayout;
    std::unique_ptr<Sampler> m_sampler;
};


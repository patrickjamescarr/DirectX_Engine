#include "pch.h"
#include "TerrainShader.h"

TerrainShader::TerrainShader()
{

}

void TerrainShader::Shutdown()
{
    // Shutdown the vertex and pixel shaders as well as the related objects.
    ShutdownShader();

    return;
}

bool TerrainShader::Render(DX::DeviceResources& deviceResources, int indexCount, DirectX::SimpleMath::Matrix worldMatrix, DirectX::SimpleMath::Matrix viewMatrix, DirectX::SimpleMath::Matrix projectionMatrix, DirectX::SimpleMath::Vector4 ambientColor, DirectX::SimpleMath::Vector4 diffuseColor, DirectX::SimpleMath::Vector3 lightDirection, ID3D11ShaderResourceView * texture)
{
    bool result;


    // Set the shader parameters that it will use for rendering.
    result = SetShaderParameters(deviceResources, worldMatrix, viewMatrix, projectionMatrix, ambientColor, diffuseColor, lightDirection, texture);
    if (!result)
    {
        return false;
    }

    // Now render the prepared buffers with the shader.
    Bind(deviceResources);

    Draw(deviceResources, indexCount);

    return true;
}

void TerrainShader::ShutdownShader()
{
}

void TerrainShader::OutputShaderErrorMessage(ID3D10Blob *, HWND, WCHAR *)
{
}

bool TerrainShader::SetShaderParameters(DX::DeviceResources & deviceResources, DirectX::SimpleMath::Matrix worldMatrix, DirectX::SimpleMath::Matrix viewMatrix, DirectX::SimpleMath::Matrix projectionMatrix, DirectX::SimpleMath::Vector4 ambientColor, DirectX::SimpleMath::Vector4 diffuseColor, DirectX::SimpleMath::Vector3 lightDirection, ID3D11ShaderResourceView * texture)
{
    return false;
}

void TerrainShader::Bind(DX::DeviceResources& deviceResources)
{
    m_inputLayout->Bind(deviceResources);
    m_vertexShader->Bind(deviceResources);
    m_pixelShader->Bind(deviceResources);
    m_sampler->Bind(deviceResources);
}

void TerrainShader::Draw(DX::DeviceResources& deviceResources, int indexCount)
{
    deviceResources.GetD3DDeviceContext()->DrawIndexed(indexCount, 0, 0);
}

bool TerrainShader::Initialize(DX::DeviceResources& deviceResources, WCHAR* vsFilename, WCHAR* psFilename)
{
    // Create the vertexShader
    m_vertexShader = std::make_unique<VertexShader>(deviceResources, vsFilename);
    m_pixelShader = std::make_unique<PixelShader>(deviceResources, psFilename);

    // Create the vertex input layout description.
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    auto vertexShaderByteCode = m_vertexShader->GetBytecode();

    m_inputLayout = std::make_unique<InputLayout>(deviceResources, layout, vertexShaderByteCode);

    m_sampler = std::make_unique< Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_WRAP, 1);

    return true;
}

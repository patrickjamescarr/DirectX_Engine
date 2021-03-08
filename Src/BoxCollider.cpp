#include "pch.h"
#include "BoxCollider.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RasterizerState.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"

BoxCollider::BoxCollider(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix transform, DirectX::SimpleMath::Vector3 dimentions, float scale)
    : Mesh(deviceResources, nullptr, nullptr, transform)
{

    halfWidth = (dimentions.x * scale) / 2;
    halfHeight = (dimentions.z * scale) / 2;

    std::vector<std::unique_ptr<Bindable>> bindables;

    // create a cube for the bounding volume of the collider
    std::vector<DirectX::VertexPositionNormalTexture> cubeVertices;
    std::vector<uint16_t> cubeIndices;
    DirectX::GeometricPrimitive::CreateBox(cubeVertices, cubeIndices, dimentions, false);

    // Create the vertex buffer
    bindables.push_back(std::make_unique<VertexBuffer>(deviceResources, cubeVertices));

    // Create the index buffer
    AddIndexBuffer(std::make_unique<IndexBuffer>(deviceResources, cubeIndices));

    // Create the vertex input layout description.
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // create the skybox vertex shader
    auto vertexShader = std::make_unique<VertexShader>(deviceResources, L"basic_vs.cso");
    auto vertexShaderByteCode = vertexShader->GetBytecode();
    AddBind(std::move(vertexShader));

    // Create the input layout
    bindables.push_back(std::make_unique<InputLayout>(deviceResources, layout, vertexShaderByteCode));

    // Create the pixel shader
    AddBind(std::make_unique<PixelShader>(deviceResources, L"basic_ps.cso"));

    // Create the rasterizer state
    bindables.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_FRONT, D3D11_FILL_WIREFRAME)); //  D3D11_FILL_WIREFRAME

    Mesh::AddBindables(std::move(bindables));
}

void BoxCollider::Draw(DX::DeviceResources & deviceResources, DirectX::FXMMATRIX accumulatedTransform) const
{
    Mesh::Draw(deviceResources, accumulatedTransform);
}

bool BoxCollider::DetectCollision(const BoxCollider & other)
{

    auto thing = other.GetTransform().Translation();

    return false;
}

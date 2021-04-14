#include "pch.h"
#include "OnScreenQuad.h"
#include "VertexBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Topology.h"
#include "DepthStencilState.h"
#include "VertexBuffer.h"
#include "OnScreenQuadConstantBufferVS.h"
#include "IndexBuffer.h"
#include "RasterizerState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;


OnScreenQuad::OnScreenQuad(
    DX::DeviceResources & deviceResources, 
    float width, 
    float height, 
    DirectX::SimpleMath::Matrix transform,
    RenderTarget* renderTarget
)
    :m_width(width), m_height(height), m_transform(transform), m_renderTarget(renderTarget)
{
    m_states = std::make_unique<CommonStates>(deviceResources.GetD3DDevice());

    float size = 0.5f;

    std::vector<VertexPositionTexture> verts;
    verts.push_back(VertexPositionTexture(Vector3(-size, size, 0.5f), Vector2(0, 0)));   // top left
    verts.push_back(VertexPositionTexture(Vector3(size, size, 0.5f), Vector2(0, 1)));     // top right
    verts.push_back(VertexPositionTexture(Vector3(size, -size, 0.5f), Vector2(1, 1)));     // bottom right
    verts.push_back(VertexPositionTexture(Vector3(-size, -size, 0.5f), Vector2(1, 0)));     // bottom left

    std::vector<uint32_t> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);

    AddBind(std::make_unique<VertexBuffer<VertexPositionTexture>>(deviceResources, verts));
    AddIndexBuffer(std::make_unique<IndexBuffer>(deviceResources, indices));

    AddBind(std::make_unique<OnScreenQuadConstantBufferVS>(deviceResources, *this));
    AddBind(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    AddBind(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::depthOff));
    AddBind(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_BACK));


    AddBind(std::make_unique<PixelShader>(deviceResources, L"quad_ps.cso"));

    auto vs = std::make_unique<VertexShader>(deviceResources, L"quad_vs.cso");
    auto vsBytecode = vs->GetBytecode();
    AddBind(std::move(vs));

    std::vector<D3D11_INPUT_ELEMENT_DESC> layout (std::begin(VertexPositionTexture::InputElements), std::end(VertexPositionTexture::InputElements));

    AddBind(std::make_unique<InputLayout>(deviceResources, layout, vsBytecode));
}

void OnScreenQuad::Draw(DX::DeviceResources & deviceResources)
{
    deviceResources.GetD3DDeviceContext()->GSSetShader(0, 0, 0);

    deviceResources.GetD3DDeviceContext()->PSSetShaderResources(0, 1, m_renderTarget->GetShaderResourceViewAddress());

    GameObject::Draw(deviceResources);
    
    // reset depth state
    deviceResources.GetD3DDeviceContext()->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    // clear the PS shader resource
    ID3D11ShaderResourceView* nullSRV[] = { nullptr };
    deviceResources.GetD3DDeviceContext()->PSSetShaderResources(0, 1, nullSRV);
}

DirectX::SimpleMath::Matrix OnScreenQuad::GetTransform() const noexcept
{
    return m_transform;
}

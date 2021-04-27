#include "pch.h"
#include "Texture3D.h"

Texture3D::Texture3D(DX::DeviceResources & deviceResource, const wchar_t* fileName, UINT slot = 0)
    : m_slot(slot)
{
    auto textureData = DX::ReadData(fileName);

    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    DX::ThrowIfFailed(
        DirectX::CreateDDSTextureFromMemory(
            deviceResource.GetD3DDevice(),
            textureData.data(),
            textureData.size(),
            resource.GetAddressOf(),
            m_textureSrv.ReleaseAndGetAddressOf()
        )
    );

    DX::ThrowIfFailed(resource.As(&m_texture));

    CD3D11_TEXTURE3D_DESC textureDesc;
    m_texture->GetDesc(&textureDesc);

    m_origin.x = float(textureDesc.Width / 2);
    m_origin.y = float(textureDesc.Height / 2);
}

void Texture3D::Bind(DX::DeviceResources & deviceResources) noexcept
{
    deviceResources.GetD3DDeviceContext()->PSSetShaderResources(m_slot, 1, m_textureSrv.GetAddressOf());
}

DirectX::SimpleMath::Vector2 Texture3D::GetOrigin() const
{
    return m_origin;
}
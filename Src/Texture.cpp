#include "pch.h"
#include "Texture.h"

Texture::Texture(DX::DeviceResources & deviceResource, const wchar_t* fileName)
{
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	DX::ThrowIfFailed(
        DirectX::CreateDDSTextureFromFile(
            deviceResource.GetD3DDevice(), 
            fileName, 
            resource.GetAddressOf(),
            m_textureSrv.ReleaseAndGetAddressOf()
        )
    );

    DX::ThrowIfFailed(resource.As(&m_texture));

    CD3D11_TEXTURE2D_DESC textureDesc;
    m_texture->GetDesc(&textureDesc);

    m_origin.x = float(textureDesc.Width / 2);
    m_origin.y = float(textureDesc.Height / 2);
}

void Texture::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->PSSetShaderResources(0, 1, m_textureSrv.GetAddressOf());
}

DirectX::SimpleMath::Vector2 Texture::GetOrigin() const
{
    return m_origin;
}

#include "pch.h"
#include "CubeTexture.h"

CubeTexture::CubeTexture(DX::DeviceResources & deviceResources, const wchar_t* fileName)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeTexture;

	// create the cube texture
	DirectX::CreateDDSTextureFromFileEx(
		deviceResources.GetD3DDevice(),
		fileName,
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		D3D11_RESOURCE_MISC_TEXTURECUBE,
		false,
		(ID3D11Resource**)cubeTexture.ReleaseAndGetAddressOf(),
		m_textureView.ReleaseAndGetAddressOf()
	);

	// get the description from the cube texture
	D3D11_TEXTURE2D_DESC cubeTextureDesc;
	cubeTexture->GetDesc(&cubeTextureDesc);

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = cubeTextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = cubeTextureDesc.MipLevels;

	DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateShaderResourceView(
		cubeTexture.Get(), 
		&shaderResourceViewDesc, 
		m_textureView.GetAddressOf()
	));
}

void CubeTexture::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->PSSetShaderResources(0, 1, m_textureView.GetAddressOf());
}

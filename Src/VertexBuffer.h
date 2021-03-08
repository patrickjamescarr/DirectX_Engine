#pragma once
#include "Bindable.h"

class VertexBuffer : public Bindable
{
public:
	struct VertexTypeNormal
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

    struct VertexTypeColour
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector2 texture;
        DirectX::SimpleMath::Vector4 colour;
    };

	VertexBuffer(DX::DeviceResources& deviceResources, const std::vector<DirectX::VertexPositionNormalTexture>& verticies);
    VertexBuffer(DX::DeviceResources& deviceResources, const std::vector<DirectX::VertexPositionColorTexture>& texverticies);
	
    void CreateVertexBuffer(const std::vector<DirectX::VertexPositionNormalTexture> & verticies, DX::DeviceResources & deviceResources);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
    void Update(DX::DeviceResources& deviceResources, const std::vector<DirectX::VertexPositionNormalTexture>& verticies);
protected:
	UINT m_vertexCount;
	UINT m_stride;    
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};


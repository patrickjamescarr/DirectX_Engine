#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(DX::DeviceResources& deviceResources, const std::vector<uint32_t>& indices);
    void CreateIndexBuffer(const std::vector<uint32_t> & indices, DX::DeviceResources & deviceResources);
    IndexBuffer(DX::DeviceResources& deviceResources, const std::vector<uint16_t>& indices);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
	UINT GetCount() const noexcept;
protected:
	UINT m_indexCount;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_IndexBuffer;
};


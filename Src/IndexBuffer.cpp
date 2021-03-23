#include "pch.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(DX::DeviceResources& deviceResources, const std::vector<uint32_t>& indices)
	: m_indexCount((UINT)indices.size())
{
    CreateIndexBuffer(indices, deviceResources);
}

IndexBuffer::IndexBuffer(DX::DeviceResources& deviceResources, const std::vector<uint16_t>& indices)
    : m_indexCount((UINT)indices.size())
{
    std::vector<uint32_t> indicesLong(indices.begin(), indices.end());

    CreateIndexBuffer(indicesLong, deviceResources);
}

void IndexBuffer::CreateIndexBuffer(const std::vector<uint32_t> & indices, DX::DeviceResources & deviceResources)
{
    m_indexCount = (UINT)indices.size();

    if (m_indexCount == 0) return;

    auto indexArray = new unsigned long[m_indexCount];

    for (int i = 0; i < m_indexCount; i++)
    {
        indexArray[i] = indices[i];
    }

    // Set up the description of the static index buffer.
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indexArray;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer));

    // release the array
    delete[] indexArray;
    indexArray = 0;
}

void IndexBuffer::Bind(DX::DeviceResources& deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

UINT IndexBuffer::GetCount() const noexcept
{
	return m_indexCount;
}

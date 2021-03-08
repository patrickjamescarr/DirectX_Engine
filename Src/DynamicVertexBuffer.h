#pragma once
#include "Bindable.h"

template <class T>
class DynamicVertexBuffer : public Bindable
{
public:
    DynamicVertexBuffer(DX::DeviceResources& deviceResources, const T& verticies)
    {
        //auto vertexCount = verticies.size();

        //// Create the vertex array.
        //auto vertexArray = new VertexTypeNormal[vertexCount];

        //assert(vertexArray);

        //// Load the vertex array and index array with data from the pre-fab
        //for (int i = 0; i < vertexCount; i++)
        //{
        //    vertexArray[i].position = DirectX::SimpleMath::Vector3(verticies[i].position.x, verticies[i].position.y, verticies[i].position.z);
        //    vertexArray[i].texture = DirectX::SimpleMath::Vector2(verticies[i].textureCoordinate.x, verticies[i].textureCoordinate.y);
        //    vertexArray[i].normal = DirectX::SimpleMath::Vector3(verticies[i].normal.x, verticies[i].normal.y, verticies[i].normal.z);
        //}

        //// Set up the description of the static vertex buffer.
        //D3D11_BUFFER_DESC vertexBufferDesc;
        //vertexBufferDesc.ByteWidth = sizeof(T) * vertexCount;
        //vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        //vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        //vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        //vertexBufferDesc.MiscFlags = 0;
        //vertexBufferDesc.StructureByteStride = 0;

        //// Give the subresource structure a pointer to the vertex data.
        //D3D11_SUBRESOURCE_DATA vertexData;
        //vertexData.pSysMem = vertexArray;
        //vertexData.SysMemPitch = 0;
        //vertexData.SysMemSlicePitch = 0;

        //DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

        //// release the array
        //delete[] vertexArray;
        //vertexArray = 0;
    }

    void Bind(DX::DeviceResources& deviceResources) noexcept override
    {
        //unsigned int stride;
        //unsigned int offset;

        //// Set vertex buffer stride and offset.
        //stride = sizeof(T);
        //offset = 0;

        //// Set the vertex buffer to active in the input assembler so it can be rendered.
        //deviceResources.GetD3DDeviceContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    }

    void Update(DX::DeviceResources& deviceResources, const T& typeData)
    {
        //D3D11_MAPPED_SUBRESOURCE mappedResource;

        //DX::ThrowIfFailed(deviceResources.GetD3DDeviceContext()->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

        //memcpy(mappedResource.pData, &typeData, sizeof(typeData));

        //deviceResources.GetD3DDeviceContext()->Unmap(m_vertexBuffer.Get(), 0);
    }
protected:
    UINT m_vertexCount;
    UINT m_stride;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};

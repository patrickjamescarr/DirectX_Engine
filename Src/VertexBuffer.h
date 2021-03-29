#pragma once
#include "Bindable.h"

template <typename T>
class VertexBuffer : public Bindable
{
public:
    VertexBuffer(DX::DeviceResources& deviceResources, const std::vector<T>& verticies)
    {
        CreateVertexBuffer(deviceResources, verticies);
    }

    void CreateVertexBuffer(DX::DeviceResources & deviceResources, const std::vector<T> & verticies)
    {
        auto vertexCount = verticies.size();

        if (vertexCount == 0) return;

        // Create the vertex array.
        auto vertexArray = new T[vertexCount];

        assert(vertexArray);

        // Load the vertex array and index array with data from the pre-fab
        std::copy(verticies.begin(), verticies.end(), vertexArray);

        // Set up the description of the static vertex buffer.
        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.ByteWidth = sizeof(T) * vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.CPUAccessFlags = 0; //D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        // Give the subresource structure a pointer to the vertex data.
        D3D11_SUBRESOURCE_DATA vertexData;
        vertexData.pSysMem = vertexArray;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        DX::ThrowIfFailed(deviceResources.GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));

        // release the array
        delete[] vertexArray;
        vertexArray = 0;
    }
    void Bind(DX::DeviceResources& deviceResources) noexcept override
    {
        unsigned int stride;
        unsigned int offset;

        // Set vertex buffer stride and offset.
        stride = sizeof(T);
        offset = 0;

        // Set the vertex buffer to active in the input assembler so it can be rendered.
        deviceResources.GetD3DDeviceContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    }

    void Update(DX::DeviceResources& deviceResources, const std::vector<T>& verticies)
    {
        CreateVertexBuffer(deviceResources, verticies);
    }
protected:
	UINT m_vertexCount;
	UINT m_stride;    
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
};


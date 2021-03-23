#include "pch.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(DX::DeviceResources & deviceResources, const std::vector<DirectX::VertexPositionNormalTexture>& verticies)
{
    CreateVertexBuffer(verticies, deviceResources);
}

void VertexBuffer::CreateVertexBuffer(const std::vector<DirectX::VertexPositionNormalTexture> & verticies, DX::DeviceResources & deviceResources)
{
    auto vertexCount = verticies.size();

    if (vertexCount == 0) return;

    // Create the vertex array.
    auto vertexArray = new VertexTypeNormal[vertexCount];

    assert(vertexArray);

    // Load the vertex array and index array with data from the pre-fab
    for (int i = 0; i < vertexCount; i++)
    {
        vertexArray[i].position = DirectX::SimpleMath::Vector3(verticies[i].position.x, verticies[i].position.y, verticies[i].position.z);
        vertexArray[i].texture = DirectX::SimpleMath::Vector2(verticies[i].textureCoordinate.x, verticies[i].textureCoordinate.y);
        vertexArray[i].normal = DirectX::SimpleMath::Vector3(verticies[i].normal.x, verticies[i].normal.y, verticies[i].normal.z);
    }

    // Set up the description of the static vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.ByteWidth = sizeof(VertexTypeNormal) * vertexCount;
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

VertexBuffer::VertexBuffer(DX::DeviceResources & deviceResources, const std::vector<DirectX::VertexPositionColorTexture>& verticies)
{
    auto vertexCount = verticies.size();

    // Create the vertex array.
    auto vertexArray = new VertexTypeColour[vertexCount];

    assert(vertexArray);

    // Load the vertex array and index array with data from the pre-fab
    for (int i = 0; i < vertexCount; i++)
    {
        vertexArray[i].position = DirectX::SimpleMath::Vector3(verticies[i].position.x, verticies[i].position.y, verticies[i].position.z);
        vertexArray[i].texture = DirectX::SimpleMath::Vector2(verticies[i].textureCoordinate.x, verticies[i].textureCoordinate.y);
        vertexArray[i].colour = DirectX::SimpleMath::Vector4(verticies[i].color.x, verticies[i].color.y, verticies[i].color.z, verticies[i].color.w);
    }

    // Set up the description of the static vertex buffer.
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.ByteWidth = sizeof(VertexTypeColour) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.CPUAccessFlags = 0; //D3D11_CPU_ACCESS_WRITE;

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

void VertexBuffer::Bind(DX::DeviceResources & deviceResources) noexcept
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexTypeNormal);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceResources.GetD3DDeviceContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
}

void VertexBuffer::Update(DX::DeviceResources & deviceResources, const std::vector<DirectX::VertexPositionNormalTexture>& verticies)
{
    CreateVertexBuffer(verticies, deviceResources);
}

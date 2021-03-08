#include "pch.h"
#include "QuadTree.h"

const int MAX_TRIANGLES = 10000;

using namespace DirectX;

QuadTree::QuadTree(DX::DeviceResources & deviceResources, MeshObject mesh)
{
    // Get the number of vertices in the mesh.
    int vertexCount = mesh.vertexCount;

    // Store the total triangle count for the vertex list.
    m_triangleCount = vertexCount / 3;

    // Copy the mesh vertices into the vertex list.
    m_vertexList.assign(mesh.verticies.begin(), mesh.verticies.end());

    float centerX, centerZ, width;

    // Calculate the center x,z and the width of the mesh.
    CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

    m_parentNode = std::make_unique<Node>();

    // Recursively build the quad tree based on the vertex list data and mesh dimensions.
    CreateTreeNode(m_parentNode.get(), centerX, centerZ, width, deviceResources.GetD3DDevice());

    // Clear the vertex list since the quad tree now has the vertices in each node.
    m_vertexList.clear();
}

void QuadTree::Bind(DX::DeviceResources & deviceResources) noexcept
{
}

int QuadTree::GetDrawCount()
{
    return 0;
}

void QuadTree::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
    // Initialize the center position of the mesh to zero.
    centerX = 0.0f;
    centerZ = 0.0f;

    // Sum all the vertices in the mesh.
    for (int i = 0; i < vertexCount; i++)
    {
        centerX += m_vertexList[i].position.x;
        centerZ += m_vertexList[i].position.z;
    }

    // And then divide it by the number of vertices to find the mid-point of the mesh.
    centerX = centerX / (float)vertexCount;
    centerZ = centerZ / (float)vertexCount;

    // Initialize the maximum and minimum size of the mesh.
    float maxWidth = 0.0f;
    float maxDepth = 0.0f;

    float minWidth = fabsf(m_vertexList[0].position.x - centerX);
    float minDepth = fabsf(m_vertexList[0].position.z - centerZ);

    // Go through all the vertices and find the maximum and minimum width and depth of the mesh.
    for (int i = 0; i < vertexCount; i++)
    {
        float width = fabsf(m_vertexList[i].position.x - centerX);
        float depth = fabsf(m_vertexList[i].position.z - centerZ);

        if (width > maxWidth) { maxWidth = width; }
        if (depth > maxDepth) { maxDepth = depth; }
        if (width < minWidth) { minWidth = width; }
        if (depth < minDepth) { minDepth = depth; }
    }

    // Find the absolute maximum value between the min and max depth and width.
    float maxX = (float)std::max(fabs(minWidth), fabs(maxWidth));
    float maxZ = (float)std::max(fabs(minDepth), fabs(maxDepth));

    // Calculate the maximum diameter of the mesh.
    meshWidth = std::max(maxX, maxZ) * 2.0f;

    return;
}

void QuadTree::CreateTreeNode(Node* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
    int numTriangles, i, count, vertexCount, index, vertexIndex;
    float offsetX, offsetZ;
    unsigned long* indices;
    bool result;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;

    //First initialize the node and set its position in the world.

    // Store the node position and size.
    node->positionX = positionX;
    node->positionZ = positionZ;
    node->width = width;

    // Initialize the triangle count to zero for the node.
    node->triangleCount = 0;

    // Initialize the vertex and index buffer to null.
    node->vertexBuffer = 0;
    node->indexBuffer = 0;

    // Initialize the children nodes of this node to null.
    node->nodes[0] = 0;
    node->nodes[1] = 0;
    node->nodes[2] = 0;
    node->nodes[3] = 0;

    //Then count the number of triangles that are in the dimensions of this node from the mesh.

    // Count the number of triangles that are inside this node.
    numTriangles = CountTriangles(positionX, positionZ, width);

    // Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
    if (numTriangles == 0)
    {
        return;
    }

    // Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
    if (numTriangles > MAX_TRIANGLES)
    {
        for (i = 0; i < 4; i++)
        {
            // Calculate the position offsets for the new child node.
            offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
            offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

            // See if there are any triangles in the new node.
            count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
            if (count > 0)
            {
                // If there are triangles inside where this new node would be then create the child node.
                node->nodes[i] = new Node;

                // Extend the tree starting from this new child node now.
                CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
            }
        }

        return;
    }

    // Case 3: If this node is not empty and the triangle count for it is less than the max then 
    // this node is at the bottom of the tree so create the list of triangles to store in it.
    node->triangleCount = numTriangles;

    // Calculate the number of vertices.
    vertexCount = numTriangles * 3;

    // Create the index array.
    indices = new unsigned long[vertexCount];

    // Initialize the index for this new vertex and index array.
    index = 0;

    // Initialize the vertex array 
    VertexPositionNormalTexture* vertices;

    // Go through all the triangles in the vertex list.
    for (i = 0; i < m_triangleCount; i++)
    {
        // If the triangle is inside this node then add it to the vertex array.
        result = IsTriangleContained(i, positionX, positionZ, width);
        if (result == true)
        {
            // Calculate the index into the vertex list.
            vertexIndex = i * 3;

            // Get the three vertices of this triangle from the vertex list.
            vertices[index].position = m_vertexList[vertexIndex].position;
            vertices[index].textureCoordinate = m_vertexList[vertexIndex].textureCoordinate;
            vertices[index].normal = m_vertexList[vertexIndex].normal;
            indices[index] = index;
            index++;

            vertexIndex++;
            vertices[index].position = m_vertexList[vertexIndex].position;
            vertices[index].textureCoordinate = m_vertexList[vertexIndex].textureCoordinate;
            vertices[index].normal = m_vertexList[vertexIndex].normal;
            indices[index] = index;
            index++;

            vertexIndex++;
            vertices[index].position = m_vertexList[vertexIndex].position;
            vertices[index].textureCoordinate = m_vertexList[vertexIndex].textureCoordinate;
            vertices[index].normal = m_vertexList[vertexIndex].normal;
            indices[index] = index;
            index++;
        }
    }

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexPositionNormalTexture) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now finally create the vertex buffer.
    device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

    // Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

    // Release the vertex and index arrays now that the data is stored in the buffers in the node.
    delete[] vertices;
    vertices = 0;

    delete[] indices;
    indices = 0;

    return;
}

int QuadTree::CountTriangles(float, float, float)
{
    return 0;
}

bool QuadTree::IsTriangleContained(int, float, float, float)
{
    return false;
}

void QuadTree::ReleaseNode(Node *)
{
}

void QuadTree::RenderNode(Node * node, ViewingFrustum * frustum, ID3D11DeviceContext * deviceContext)
{
    bool result;
    int count, i, indexCount;
    unsigned int stride, offset;

    // Check to see if the node can be viewed, height doesn't matter in a quad tree.
    result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));

    // If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
    if (!result)
    {
        return;
    }

    // If it can be seen then check all four child nodes to see if they can also be seen.
    count = 0;
    for (i = 0; i < 4; i++)
    {
        if (node->nodes[i] != 0)
        {
            count++;
            // RenderNode(node->nodes[i], frustum, deviceContext, shader);
        }
    }

    // If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
    if (count != 0)
    {
        return;
    }

    // Otherwise if this node can be seen and has triangles in it then render these triangles.

    // Set vertex buffer stride and offset.
    stride = sizeof(VertexPositionNormalTexture);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Determine the number of indices in this node.
    indexCount = node->triangleCount * 3;

    // Call the terrain shader to render the polygons in this node.
    // shader->RenderShader(deviceContext, indexCount);


    // Increase the count of the number of polygons that have been rendered during this frame.
    m_drawCount += node->triangleCount;

    return;
}

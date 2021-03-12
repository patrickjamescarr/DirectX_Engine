#include "pch.h"
#include "QuadTree.h"

const int MAX_TRIANGLES = 5000;

using namespace DirectX;
using namespace DirectX::SimpleMath;

QuadTree::QuadTree(DX::DeviceResources & deviceResources, MeshObject mesh, std::vector<std::unique_ptr<Bindable>> binds)
    : m_bindables(std::move(binds))
{
    Update(deviceResources, mesh);
}

void QuadTree::Update(DX::DeviceResources & deviceResources, MeshObject &mesh)
{
    // Set the scale for use in frustum culling calcs
    m_scale = mesh.scale;

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
    //m_vertexList.clear();
}


void QuadTree::Render(DX::DeviceResources & deviceResources, ViewingFrustum* frustum) noexcept
{
    // Reset the number of triangles that are drawn for this frame.
    m_drawCount = 0;

    // Render each node that is visible starting at the parent node and moving down the tree.
    RenderNode(m_parentNode.get(), frustum, deviceResources);

    if (m_parentNode->nodes.empty())
    {
        return;
    }

    if (ImGui::Begin("Node Culling"))
    {
        ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
        ImGui::Text("Node 1: Position x:%f z:%f Culled: %d", m_parentNode->nodes[0]->positionX, m_parentNode->nodes[0]->positionZ, m_parentNode->nodes[0]->culled);
        ImGui::Text("Node 2: Position x:%f z:%f Culled: %d", m_parentNode->nodes[1]->positionX, m_parentNode->nodes[1]->positionZ, m_parentNode->nodes[1]->culled);
        ImGui::Text("Node 3: Position x:%f z:%f Culled: %d", m_parentNode->nodes[2]->positionX, m_parentNode->nodes[2]->positionZ, m_parentNode->nodes[2]->culled);
        ImGui::Text("Node 4: Position x:%f z:%f Culled: %d", m_parentNode->nodes[3]->positionX, m_parentNode->nodes[3]->positionZ, m_parentNode->nodes[3]->culled);
    }


    ImGui::End();
}

bool QuadTree::DetectCollision(DirectX::SimpleMath::Vector3 objectPosition, DirectX::SimpleMath::Matrix transform, CollisionIntersection& collisionIntersection)
{
    return CheckNodeForCollision(m_parentNode.get(), objectPosition, transform, collisionIntersection);
}

bool QuadTree::CheckNodeForCollision(Node * parentNode, DirectX::SimpleMath::Vector3 objectPosition, DirectX::SimpleMath::Matrix transform, CollisionIntersection& collisionIntersection)
{
    for (auto& node : parentNode->nodes)
    {
        // get scaled node position coordinates and size
        // NOTE: could alternatively pass the pre-transformed position in 
        auto nodePositionX = node->positionX * m_scale;
        auto nodePositionZ = node->positionZ * m_scale;
        auto size = node->width * m_scale;

        // check to see if object position lies within the node

        if (objectPosition.x < (nodePositionX - (size / 2))) continue; // left
        if (objectPosition.z < (nodePositionZ - (size / 2))) continue; // top
        if (objectPosition.x > (nodePositionX + (size / 2))) continue; // right
        if (objectPosition.z > (nodePositionZ + (size / 2))) continue; // bottom

        // point lies within this node

        // if this node doesnt contain any children then check the verticies 
        // within it for the collision point, otherwise check child nodes

        if (!node->nodes.empty())
        {
            return CheckNodeForCollision(node.get(), objectPosition, transform, collisionIntersection);
        }

        auto vertexCount = node->vertexEndIndex - node->vertexStartIndex;
        auto triangleCount = vertexCount / 3;

        auto rayDirection = Vector3(0.0f, 1.0f, 0.0f);

        // find the collision point
        for (int i = 0; i < triangleCount; i++)
        {
            // Calculate the starting index into the vertex list.
            auto vertexIndex = node->vertexStartIndex + (i * 3);

            // Get the three vertices of this triangle from the vertex list.

            auto scaleTransform = Matrix::CreateScale(m_scale);

            auto v1 = Vector3::Transform(m_vertexList[vertexIndex].position, transform);

            vertexIndex++;
            auto v2 = Vector3::Transform(m_vertexList[vertexIndex].position, transform);

            vertexIndex++;
            auto v3 = Vector3::Transform(m_vertexList[vertexIndex].position, transform);

            auto collision = m_collision.rayTriangleIntersectMT(
                objectPosition,
                rayDirection,
                v1,
                v2,
                v3,
                collisionIntersection.t, 
                collisionIntersection.u, 
                collisionIntersection.v
            );

            if (collision)
            {
                return true;
            }
        }
    }

    return false;
}

int QuadTree::GetDrawCount()
{
    return m_drawCount;
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

    // Count the number of triangles that are inside this node.
    int numTriangles = CountTriangles(positionX, positionZ, width);

    // Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
    if (numTriangles == 0)
    {
        return;
    }

    // Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
    if (numTriangles > MAX_TRIANGLES)
    {
        for (int i = 0; i < 4; i++)
        {
            // Calculate the position offsets for the new child node.
            float offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
            float offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);

            // See if there are any triangles in the new node.
            int triangleCount = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));

            if (triangleCount > 0)
            {
                // If there are triangles inside where this new node would be then create the child node.
                node->nodes.push_back(std::make_unique<Node>());

                // Extend the tree starting from this new child node now.
                CreateTreeNode(node->nodes[i].get(), (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device);
            }
        }

        return;
    }

    // Case 3: If this node is not empty and the triangle count for it is less than the max then 
    // this node is at the bottom of the tree so create the list of triangles to store in it.
    node->triangleCount = numTriangles;

    // Calculate the number of vertices.
    int vertexCount = numTriangles * 3;

    // Create the index array.
    unsigned long* indices = new unsigned long[vertexCount];

    // Initialize the index for this new vertex and index array.
    int index = 0;

    // Initialize the vertex array 
    VertexPositionNormalTexture* vertices;

    vertices = new VertexPositionNormalTexture[vertexCount];

    int vertexIndex;
    bool firstIndex = true;

    // Go through all the triangles in the vertex list.
    for (int i = 0; i < m_triangleCount; i++)
    {
        // If the triangle is inside this node then add it to the vertex array.
        bool result = IsTriangleContained(i, positionX, positionZ, width);
        if (result == true)
        {
            // Calculate the index into the vertex list.
            vertexIndex = i * 3;

            // Set the starting index for this nodes verticies in the vertex list
            if (firstIndex)
            {
                node->vertexStartIndex = vertexIndex;
                firstIndex = false;
            }

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

    // Set the end index for this nodes verticies in the vertex list
    node->vertexEndIndex = vertexIndex;

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

// Goes through the list of triangles from the terrain data and 
// determines which ones are inside the given dimensions.
int QuadTree::CountTriangles(float positionX, float positionZ, float width)
{
    // Initialize the count to zero.
    int count = 0;

    // Go through all the triangles in the entire mesh and check which ones should be inside this node.
    for (int i = 0; i < m_triangleCount; i++)
    {
        // If the triangle is inside the node then increment the count by one.
        auto result = IsTriangleContained(i, positionX, positionZ, width);
        if (result == true)
        {
            count++;
        }
    }

    return count;
}

// Calculates if the given triangle is completely inside the input cube dimensions or not.
bool QuadTree::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
    // Calculate the radius of this node.
    float radius = width / 2.0f;

    // Get the index into the vertex list.
    int vertexIndex = index * 3;

    // Get the three vertices of this triangle from the vertex list.
    float x1 = m_vertexList[vertexIndex].position.x;
    float z1 = m_vertexList[vertexIndex].position.z;
    vertexIndex++;

    float x2 = m_vertexList[vertexIndex].position.x;
    float z2 = m_vertexList[vertexIndex].position.z;
    vertexIndex++;

    float x3 = m_vertexList[vertexIndex].position.x;
    float z3 = m_vertexList[vertexIndex].position.z;

    // Check to see if the minimum of the x coordinates of the triangle is inside the node.
    float minimumX = std::min(x1, std::min(x2, x3));
    if (minimumX > (positionX + radius))
    {
        return false;
    }

    // Check to see if the maximum of the x coordinates of the triangle is inside the node.
    float maximumX = std::max(x1, std::max(x2, x3));
    if (maximumX < (positionX - radius))
    {
        return false;
    }

    // Check to see if the minimum of the z coordinates of the triangle is inside the node.
    float minimumZ = std::min(z1, std::min(z2, z3));
    if (minimumZ > (positionZ + radius))
    {
        return false;
    }

    // Check to see if the maximum of the z coordinates of the triangle is inside the node.
    float maximumZ = std::max(z1, std::max(z2, z3));
    if (maximumZ < (positionZ - radius))
    {
        return false;
    }

    return true;
}

void QuadTree::RenderNode(Node * node, ViewingFrustum * frustum, DX::DeviceResources& deviceResources)
{
    // Check to see if the node can be viewed, height doesn't matter in a quad tree.
    bool drawNode = frustum->CheckCube(node->positionX * m_scale, 0.0f, node->positionZ * m_scale, (node->width / 2.0f) * m_scale);

    node->culled = !drawNode;

    // If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
    if (!drawNode)
    {
        return;
    }

    // If it can be seen then check all four child nodes to see if they can also be seen.
    if (!node->nodes.empty())
    {
        for (auto& node : node->nodes)
        {
            RenderNode(node.get(), frustum, deviceResources);
        }

        // If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
        return;
    }

    // Otherwise if this node can be seen and has triangles in it then render these triangles.

    // Set vertex buffer stride and offset.
    unsigned int stride = sizeof(VertexPositionNormalTexture);
    unsigned int offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceResources.GetD3DDeviceContext()->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceResources.GetD3DDeviceContext()->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceResources.GetD3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Determine the number of indices in this node.
    int indexCount = node->triangleCount * 3;

	// bind all of this obejcts bindables to the pipeline
	for (auto& b : m_bindables)
	{
		b->Bind(deviceResources);
	}

	// draw the object
    deviceResources.GetD3DDeviceContext()->DrawIndexed(indexCount, 0, 0);

    // Increase the count of the number of polygons that have been rendered during this frame.
    m_drawCount += node->triangleCount;

    return;
}



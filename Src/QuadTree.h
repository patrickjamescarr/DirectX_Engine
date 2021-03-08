#pragma once
#include "Terrain.h"
#include "Bindable.h"
#include "ViewingFrustum.h"

class QuadTree : public Bindable
{
private:
    struct Node
    {
        float positionX, positionZ, width;
        int triangleCount;
        ID3D11Buffer *vertexBuffer, *indexBuffer;
        Node* nodes[4];
    };
public:
    QuadTree(DX::DeviceResources& deviceResources, MeshObject terrainMesh);
    ~QuadTree() {};

    void Bind(DX::DeviceResources& deviceResources) noexcept override;

    int GetDrawCount();
private:
    void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
    void CreateTreeNode(Node* node, float positionX, float positionZ, float width, ID3D11Device* device);
    int CountTriangles(float, float, float);
    bool IsTriangleContained(int, float, float, float);
    void ReleaseNode(Node*);
    void RenderNode(Node* nodeType, ViewingFrustum* frustum, ID3D11DeviceContext* deviceContext);
private:
    int m_triangleCount, m_drawCount;
    std::vector<DirectX::VertexPositionNormalTexture> m_vertexList;
    std::unique_ptr<Node> m_parentNode;
    std::vector<std::unique_ptr<Bindable>> m_bindables;
};


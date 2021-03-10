#pragma once
#include "Bindable.h"
#include "ViewingFrustum.h"
#include "Types.h"

class QuadTree
{
private:
    struct Node
    {
        float positionX, positionZ, width;
        int triangleCount;
        ID3D11Buffer *vertexBuffer, *indexBuffer;
        std::vector<std::unique_ptr<Node>> nodes;
        bool culled = true;
    };
public:
    QuadTree(DX::DeviceResources& deviceResources, MeshObject terrainMesh, std::vector<std::unique_ptr<Bindable>> binds);
    void Update(DX::DeviceResources & deviceResources, MeshObject &mesh);
    ~QuadTree() {};

    void Render(DX::DeviceResources& deviceResources, ViewingFrustum* frustum) noexcept;

    int GetDrawCount();
private:
    void CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth);
    void CreateTreeNode(Node* node, float positionX, float positionZ, float width, ID3D11Device* device);
    int CountTriangles(float positionX, float positionZ, float width);
    bool IsTriangleContained(int index, float positionX, float positionZ, float width);
    void RenderNode(Node* nodeType, ViewingFrustum* frustum, DX::DeviceResources& deviceResources);
private:
    int m_triangleCount;
    int m_drawCount;
    std::vector<DirectX::VertexPositionNormalTexture> m_vertexList;
    std::unique_ptr<Node> m_parentNode;
    std::vector<std::unique_ptr<Bindable>> m_bindables;
    float m_scale;
};


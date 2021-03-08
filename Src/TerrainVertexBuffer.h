#pragma once
#include "VertexBuffer.h"
#include "Bindable.h"
#include "ModelLoader.h"

class TerrainVertexBuffer : public Bindable
{
public:
    TerrainVertexBuffer(DX::DeviceResources& deviceResources, const MeshObject& mesh);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
    void Update(DX::DeviceResources & deviceResources);

    const std::shared_ptr<VertexBuffer>& GetVertexBuffer() {
        return m_terrainBuffer;
    }

private:
    std::shared_ptr<VertexBuffer> m_terrainBuffer;
    const MeshObject& m_mesh;
};


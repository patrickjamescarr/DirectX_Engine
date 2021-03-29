#include "pch.h"
#include "MarchingCubesMesh.h"
#include "GeometryShader.h"

using namespace DirectX::SimpleMath;

MarchingCubesMesh::MarchingCubesMesh(DX::DeviceResources & deviceResources, Light * sceneLight, DirectX::SimpleMath::Matrix transform, const wchar_t * textureFileName, const wchar_t * vertexShaderFileName, const wchar_t * pixelShaderFileName)
    :Mesh(deviceResources, sceneLight, transform, textureFileName, vertexShaderFileName, pixelShaderFileName), 
    m_deviceResources(deviceResources)
{

    auto gs = std::make_unique<GeometryShader>(deviceResources, L"test_gs.cso");
    AddBind(std::move(gs));

    m_terrainFunction = std::make_unique<TerrainDensityFunction>(&m_brownianFunction);
    m_function = m_terrainFunction.get();
    m_functionType = Terrain;

    m_marchingCubes = std::make_unique<MarchingCubes>(m_function);

    auto modelMesh = m_marchingCubes->Generate(m_width, m_height, m_depth);

    // create the vertex buffer and store a pointer to it
    auto vertexBuffer = std::make_unique<VertexBuffer<DirectX::VertexPositionNormalTexture>>(deviceResources, modelMesh.verticies);
    m_vertexBuffer = vertexBuffer.get();
    AddBind(std::move(vertexBuffer));

    // Create the index buffer and store a pointer to it
    auto indexBuffer = std::make_unique<IndexBuffer>(deviceResources, modelMesh.indices);
    m_indexBuffer = indexBuffer.get();
    AddIndexBuffer(std::move(indexBuffer));
}

void MarchingCubesMesh::Update()
{
    if (ImGui::Begin("Marching Cubes"))
    {
        ImGui::SliderInt("Width", &m_width, 1, 200);
        ImGui::SliderInt("Depth", &m_height, 1, 200);
        ImGui::SliderInt("Height", &m_depth, 1, 200);

        ImGui::SliderFloat("Cube Scale", &m_marchingCubes->m_scale, 0.0f, 1.0f);
        ImGui::SliderFloat("Iso Level", &m_marchingCubes->m_isoLevel, 0.0f, 1.0f);

        static const char * listItems[]{ "Simplex", "Brownian", "Terrain"};

        ImGui::ListBox("Terrain Type", &m_functionType, listItems, IM_ARRAYSIZE(listItems));

        switch (m_functionType)
        {
        case Brownian:
            m_function = &m_brownianFunction;
            break;
        case Terrain:
            m_function = m_terrainFunction.get();
            break;
        case Simplex:
            m_function = &m_simplexFunction;
            break;
        default:
            break;
        }

        if (m_functionType == Brownian || m_functionType == Terrain)
        {
            ShowFBMControls();
        }

        if (ImGui::Button("Apply"))
        {
            m_marchingCubes->SetFunction(m_function);

            auto modelMesh = m_marchingCubes->Generate(m_width, m_height, m_depth);
            m_vertexBuffer->Update(m_deviceResources, modelMesh.verticies);
            m_indexBuffer->CreateIndexBuffer(modelMesh.indices, m_deviceResources);
        }
    }

    ImGui::End();
}

void MarchingCubesMesh::ShowFBMControls()
{
    ImGui::SliderInt("Octaves", &m_brownianFunction.m_octaves, 1, 12);
    ImGui::SliderFloat("Amplitude", &m_brownianFunction.m_amplitude, 0.0f, 5.0f);
    ImGui::SliderFloat("Value", &m_brownianFunction.m_value, -1.0f, 1.0f);
    ImGui::SliderFloat("Lacunarity", &m_brownianFunction.m_lacunarity, 0.0f, 5.0f);
    ImGui::SliderFloat("Gain", &m_brownianFunction.m_gain, -1.0f, 1.0f);
}

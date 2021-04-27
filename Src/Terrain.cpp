#include "pch.h"
#include "Terrain.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "TerrainVertexBuffer.h"
#include "Sampler.h"
#include "RasterizerState.h"
#include "InputLayout.h"
#include "VertexBuffer.h"
#include "Topology.h"
#include "DepthStencil.h"
#include "DepthStencilState.h"
#include "TransformConstantBufferVS.h"
#include "LightConstantBuffer.h"
#include "Texture.h"
#include "MarchingCubes.h"

using namespace DirectX::SimpleMath;

Terrain::Terrain(
    DX::DeviceResources& deviceResources, 
    Light* light, 
    const wchar_t* textureFileName, 
    DirectX::SimpleMath::Matrix transform, 
    int terrainWidth, 
    int terrainHeight, 
    float scale,
    PlayerCamera* playerCamera,
    ViewingFrustum* frustum
)
    : Mesh{ transform }, 
    m_terrainWidth(terrainWidth), 
    m_terrainHeight(terrainHeight),
    m_scale(scale),
    m_deviceResources(deviceResources),
    m_camera(playerCamera),
    m_frustum(frustum)
{

    //MarchingCubes mc;
    //mc.GenerateTerrain(10, 10, 10);

    // Create the structure to hold the terrain data.
    m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];

    if (!m_heightMap)
    {
        return;
    }

    CalculateHeightMapTextureCoordinates();

	//even though we are generating a flat terrain, we still need to normalise it. 
	// Calculate the normals for the terrain data.
	auto result = CalculateNormals();
	if (!result)
	{
		return;
	}

    m_terrain = m_modelLoader.CreateTerrain(m_terrainWidth, m_terrainHeight, m_heightMap, m_scale);

    std::vector<std::unique_ptr<Bindable>> bindables;

    // Create the vertex shader
    auto vertexShader = std::make_unique<VertexShader>(deviceResources, L"terrain_vs.cso");
    auto vertexShaderByteCode = vertexShader->GetBytecode();
    bindables.push_back(std::move(vertexShader));

    // Create the pixel shader
    bindables.push_back(std::make_unique<PixelShader>(deviceResources, L"light_ps.cso"));

    bindables.push_back(std::make_unique<Sampler>(deviceResources, D3D11_TEXTURE_ADDRESS_WRAP));

    // Create the vertex input layout description.
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout{
        { "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                               D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,    D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    // Create the input layout
    bindables.push_back(std::make_unique<InputLayout>(deviceResources, layout, vertexShaderByteCode));

    // Create the rasterizer state
    bindables.push_back(std::make_unique<RasterizerState>(deviceResources, D3D11_CULL_FRONT)); //  D3D11_FILL_WIREFRAME

    bindables.push_back(std::make_unique<Topology>(deviceResources, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    bindables.push_back(std::make_unique<DepthStencilState>(deviceResources, DepthStencilState::default));

    bindables.push_back(std::make_unique<TransformConstantBufferVS>(deviceResources, *this));

    bindables.push_back(std::make_unique<LightConstantBuffer>(deviceResources, light));
    
    bindables.push_back(std::make_unique<Texture>(deviceResources, textureFileName));

    m_quadTree = std::make_unique<QuadTree>(deviceResources, m_terrain, std::move(bindables));
}

void Terrain::CalculateHeightMapTextureCoordinates()
{
    // this is how we calculate the texture coordinates
    // first calculate the step size there will be between vertices. 
    float textureCoordinatesStep = 5.0f / m_terrainWidth;  //tile 5 times across the terrain. 

    // Initialise the data in the height map (flat).
    int index;
    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            m_heightMap[index].x = (float)i;
            m_heightMap[index].y = 0.0f; // Zero height for flat terrain
            m_heightMap[index].z = (float)j;

            //and use this step to calculate the texture coordinates for this point on the terrain.
            m_heightMap[index].u = (float)i * textureCoordinatesStep;
            m_heightMap[index].v = (float)j * textureCoordinatesStep;

        }
    }
}

Terrain::~Terrain()
{
    delete[] m_heightMap;
    m_heightMap = 0;
}

void Terrain::Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
    m_accumulatedTransform = accumulatedTransform;

    m_quadTree->Render(deviceResources, m_frustum);

    CollisionIntersection collisionIntersection;

    auto collision = m_quadTree->DetectCollision(m_camera->getPosition(), Mesh::GetTransform(), collisionIntersection);

    if (collision)
    {
        m_camera->adjustHeight(std::abs(collisionIntersection.t));
    }
}

void Terrain::Update()
{
    DisplayGUI();
}

bool Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	DirectX::SimpleMath::Vector3* normals;

	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new DirectX::SimpleMath::Vector3[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (m_terrainHeight - 1); j++)
	{
		for (i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j < m_terrainHeight; j++)
	{
		for (i = 0; i < m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void Terrain::GenerateHeightMap()
{
    int index;

    //loop through the terrain and set the hieghts how we want. This is where we generate the terrain
    //in this case I will run a sin-wave through the terrain in one axis.

    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            m_heightMap[index].x = (float)i;
            m_heightMap[index].y = GenerateRandomHeightValue();
            m_heightMap[index].z = (float)j;
        }
    }
}

void Terrain::FlattenTerrain()
{
    int index;

    //loop through the terrain and set the hieghts how we want. This is where we generate the terrain
    //in this case I will run a sin-wave through the terrain in one axis.

    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            m_heightMap[index].x = (float)i;
            m_heightMap[index].y = 0;
            m_heightMap[index].z = (float)j;
        }
    }
}

void Terrain::ApplySmoothing()
{
    int index;

    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            auto neighbourCount = 0;
            float avgHeight = 0.0f;

            //top left
            if (i > 0 && j > 0)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * (j - 1)) + (i - 1)].y;
            }

            // top middle
            if (j > 0)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * (j - 1)) + i].y;
            }

            // top right
            if (j < m_terrainHeight - 1 && i > 0)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * (j + 1)) + (i - 1)].y;
            }

            // left
            if (i > 0)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * j) + i - 1].y;
            }

            // right 
            if (i < m_terrainWidth - 1)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * j) + i + 1].y;
            }

            // bottom left
            if (i > 0 && j < m_terrainHeight - 1)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * (j + 1)) + (i - 1)].y;
            }

            // bottom middle
            if (j < m_terrainHeight - 1)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * (j + 1)) + i].y;
            }

            // bottm right
            if (j < m_terrainHeight - 1 && i < m_terrainWidth - 1)
            {
                neighbourCount++;
                avgHeight += m_heightMap[(m_terrainHeight * (j + 1)) + (i + 1)].y;
            }

            avgHeight /= neighbourCount;

            m_heightMap[index].x = (float)i;
            m_heightMap[index].y = avgHeight;
            m_heightMap[index].z = (float)j;
        }
    }
}

void Terrain::ApplyFaulting()
{
    CreateFaultLine(m_faultStepSize);

    m_faultStepSize /= 1.01;
}

void Terrain::CreateFaultLine(float stepSize)
{
    // first random point
    auto firstPoint = rand() / (RAND_MAX / ((m_terrainWidth - 1) * (m_terrainHeight - 1)));
    // second random point
    auto secondPoint = rand() / (RAND_MAX / ((m_terrainWidth - 1) * (m_terrainHeight - 1)));

    // create vector
    auto firstPointVector = Vector3(m_heightMap[firstPoint].x, 0, m_heightMap[firstPoint].z);
    auto secondPointVector = Vector3(m_heightMap[secondPoint].x, 0, m_heightMap[secondPoint].z);

    auto faultVector = firstPointVector - secondPointVector;

    int index;

    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            auto currentPointVector = firstPointVector - Vector3(m_heightMap[index].x, 0, m_heightMap[index].z);

            auto crossProduct = faultVector.Cross(currentPointVector);

            auto faultAdjustment = m_faultUp ? m_heightMap[index].y - stepSize : m_heightMap[index].y + stepSize;

            m_heightMap[index].y = crossProduct.y < 0 ? faultAdjustment : m_heightMap[index].y;
        }
    }

    m_faultUp = !m_faultUp;
}

void Terrain::CreateFmbFaultLine(float stepSize)
{
    // first random point
    auto firstPoint = rand() / (RAND_MAX / ((m_terrainWidth - 1) * (m_terrainHeight - 1)));
    // second random point
    auto secondPoint = rand() / (RAND_MAX / ((m_terrainWidth - 1) * (m_terrainHeight - 1)));

    // create vector
    auto firstPointVector = Vector3(m_heightMap[firstPoint].x, 0, m_heightMap[firstPoint].z);
    auto secondPointVector = Vector3(m_heightMap[secondPoint].x, 0, m_heightMap[secondPoint].z);

    auto faultVector = firstPointVector - secondPointVector;

    int index;

    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            auto currentPointVector = firstPointVector - Vector3(m_heightMap[index].x, 0, m_heightMap[index].z);

            auto crossProduct = faultVector.Cross(currentPointVector);

            auto faultAdjustment = m_faultUp ? m_heightMap[index].y - stepSize : m_heightMap[index].y + stepSize;

            m_heightMap[index].y = crossProduct.y < 0 ? faultAdjustment : m_heightMap[index].y;
        }
    }

    m_faultUp = !m_faultUp;
}

void Terrain::Generate()
{
    switch (m_selectedTerrain)
    {
    case Terrain::TerrainType::Flat:
        FlattenTerrain();
        break;
    case Terrain::TerrainType::Smooth:
        ApplySmoothing();
        break;
    case Terrain::TerrainType::RandomHeight:
        GenerateHeightMap();
        break;
    case Terrain::TerrainType::Fault:
        ApplyFaulting();
        break;
    case Terrain::TerrainType::Noise:
        GenerateNoise(Perlin);
        break;
    case Terrain::TerrainType::FBMTerrain:
        GenerateNoise(FBMNoise);
        break;
    default:
        break;
    }

    auto result = CalculateNormals();
    if (!result)
    {
        return;
    }

    m_terrain = m_modelLoader.CreateTerrain(m_terrainWidth, m_terrainHeight, m_heightMap, m_scale);

    m_quadTree->Update(m_deviceResources, m_terrain);
}

bool Terrain::DetectCameraCollision() const
{
    int indexCount = 0; // index counter 
    int v1, v2, v3, v4, v5, v6; //geometric indices.

    auto rayOrigin = m_camera->getPosition();

    auto rayDirection = Vector3(0.0f, 1.0f, 0.0f);

    bool hit = false;

    for (int j = 0; j < (m_terrainHeight - 1); j++)
    {
        for (int i = 0; i < (m_terrainWidth - 1); i++)
        {
            v1 = indexCount;
            v2 = v1 + 1;
            v3 = v2 + 1;
            v4 = v3 + 1;
            v5 = v4 + 1;
            v6 = v5 + 1;

            indexCount += 6;

            float t, u, v;

            Vector3 intersection;

            auto transform = Mesh::GetTransform();

            hit = m_collider.rayTriangleIntersectMT(
                rayOrigin,
                rayDirection,
                Vector3::Transform(m_terrain.verticies[v1].position, transform),
                Vector3::Transform(m_terrain.verticies[v2].position, transform),
                Vector3::Transform(m_terrain.verticies[v3].position, transform),
                t, u, v
            );

            if (hit)
            {
                m_camera->adjustHeight(std::abs(t));
                return true;
            }

            hit = m_collider.rayTriangleIntersectMT(
                rayOrigin,
                rayDirection,
                Vector3::Transform(m_terrain.verticies[v4].position, transform),
                Vector3::Transform(m_terrain.verticies[v5].position, transform),
                Vector3::Transform(m_terrain.verticies[v6].position, transform),
                t, u, v
            );

            if (hit)
            {
                m_camera->adjustHeight(std::abs(t));
                return true;
            }
        }
    }

    return false;
}

void Terrain::GenerateNoise(NoiseType noiseType)
{
    int index;

    float scale = 0.03;

    for (int j = 0; j < m_terrainHeight; j++)
    {
        for (int i = 0; i < m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            if (noiseType == Perlin)
            {
                auto point = Vector3(m_heightMap[index].x, m_heightMap[index].y, m_heightMap[index].z) * scale;
                m_heightMap[index].y = m_noise.Generate(point) * 2.0f;
            }
            else
            {
                m_heightMap[index].y = fbm.Generate(Vector2(m_heightMap[index].x * scale, m_heightMap[index].z * scale), fbm_val, fbm_amp, fbm_octaves, fbm_lacunarity, fbm_gain);
            }
        }
    }
}

float Terrain::GenerateRandomHeightValue()
{
    auto createPeak = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) < 0.05f;

    return createPeak ? static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / m_randomHeightValue)) : 0.0f;
}

void Terrain::DisplayGUI() noexcept
{
    if (ImGui::Begin("Terrain Parameters"))
    {
        ImGui::SliderFloat("Max Random Height", &m_randomHeightValue, 0.0f, 10.0f);
        ImGui::SliderFloat("Fault Step Size", &m_faultStepSize, 0.0f, 1.0f);

        static const char * listItems[]{ "Flat", "Random Height", "Fault", "Smooth", "Simplex Noise", "Fractal Brownian Motion" };

        ImGui::ListBox("Terrain Type", &m_selectedTerrain, listItems, IM_ARRAYSIZE(listItems));

        if (m_selectedTerrain == FBMTerrain)
        {
            ImGui::SliderInt("Octaves", &fbm_octaves, 1, 12);
            ImGui::SliderFloat("Amplitude", &fbm_amp, 0.0f, 5.0f);
            ImGui::SliderFloat("Value", &fbm_val, -1.0f, 1.0f);
            ImGui::SliderFloat("Lacunarity", &fbm_lacunarity, 0.0f, 5.0f);
            ImGui::SliderFloat("Gain", &fbm_gain, -1.0f, 1.0f);
        }

        if (ImGui::Button("Apply"))
        {
            Generate();
        }
    }

    ImGui::End();
}


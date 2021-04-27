#include "pch.h"
#include "ModelLoader.h"
#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;

MeshObject ModelLoader::LoadModel(const char* fileName)
{
	MeshObject mesh;

	std::vector<XMFLOAT3> verts;
	std::vector<XMFLOAT3> norms;
	std::vector<XMFLOAT2> texCs;
	std::vector<unsigned int> faces;

	FILE* file;
	errno_t err;
	err = fopen_s(&file, fileName, "r");

	assert("Opening model file" && err == 0);

	while (true)
	{
		char lineHeader[128];

		// Read first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
		{
			break; // exit loop
		}
		else // Parse
		{
			if (strcmp(lineHeader, "v") == 0) // Vertex
			{
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) // Tex Coord
			{
				XMFLOAT2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				texCs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) // Normal
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) // Face
			{
				unsigned int face[9];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &face[0], &face[1], &face[2],
					&face[3], &face[4], &face[5],
					&face[6], &face[7], &face[8]);

				assert("Parser error, or not triangle faces" && matches == 9);

				for (int i = 0; i < 9; i++)
				{
					faces.push_back(face[i]);
				}
			}
		}
	}

	int vIndex = 0, nIndex = 0, tIndex = 0;
	int numFaces = (int)faces.size() / 9;

	//// Create the model using the vertex count that was read in.
	mesh.vertexCount = numFaces * 3;
	//	model = new ModelType[vertexCount];

		// "Unroll" the loaded obj information into a list of triangles.
	for (int f = 0; f < (int)faces.size(); f += 3)
	{
		VertexPositionNormalTexture tempVertex;
		tempVertex.position.x = verts[(faces[f + 0] - 1)].x;
		tempVertex.position.y = verts[(faces[f + 0] - 1)].y;
		tempVertex.position.z = verts[(faces[f + 0] - 1)].z;

		tempVertex.textureCoordinate.x = texCs[(faces[f + 1] - 1)].x;
		tempVertex.textureCoordinate.y = texCs[(faces[f + 1] - 1)].y;

		tempVertex.normal.x = norms[(faces[f + 2] - 1)].x;
		tempVertex.normal.y = norms[(faces[f + 2] - 1)].y;
		tempVertex.normal.z = norms[(faces[f + 2] - 1)].z;

		//increase index count
		mesh.verticies.push_back(tempVertex);

		int tempIndex;
		tempIndex = vIndex;
		mesh.indices.push_back(tempIndex);
		vIndex++;
	}

	mesh.indexCount = vIndex;

	verts.clear();
	norms.clear();
	texCs.clear();
	faces.clear();

	return mesh;
}

MeshObject ModelLoader::LoadModelFromTextFile(const char * fileName)
{
    MeshObject mesh;

    std::ifstream fin;
    char input;
    int i;

    // Open the model file.  
    fin.open(fileName);
    assert("Opening model file" && !fin.fail());

    // Read up to the value of vertex count.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }

    // Read in the vertex count.
    fin >> mesh.vertexCount;

    // Set the number of indices to be the same as the vertex count.
    mesh.indexCount = mesh.vertexCount;

    // Read up to the beginning of the data.
    fin.get(input);
    while (input != ':')
    {
        fin.get(input);
    }
    fin.get(input);
    fin.get(input);

    // Read in the vertex data.
    for (i = 0; i < mesh.vertexCount; i++)
    {
        VertexPositionNormalTexture tempVertex;

        fin >> tempVertex.position.x >> tempVertex.position.y >> tempVertex.position.z;
        fin >> tempVertex.textureCoordinate.x >> tempVertex.textureCoordinate.y;
        fin >> tempVertex.normal.x >> tempVertex.normal.y >> tempVertex.normal.z;

        mesh.verticies.push_back(tempVertex);
        mesh.indices.push_back(i);
    }

    // Close the model file.
    fin.close();

    return mesh;
}

MeshObject ModelLoader::CreateSphere(float diameter)
{
    auto sphere = MeshObject();

    std::vector<uint16_t> indicies;

    GeometricPrimitive::CreateSphere(sphere.verticies, indicies, diameter, 8);

    sphere.indices.assign(indicies.begin(), indicies.end());

    return sphere;
}

MeshObject ModelLoader::CreateCube(DirectX::SimpleMath::Vector3 dims)
{
    auto cube = MeshObject();

    std::vector<uint16_t> indicies;

    GeometricPrimitive::CreateBox(cube.verticies, indicies, dims);

    cube.indices.assign(indicies.begin(), indicies.end());

    return cube;
}

MeshObject ModelLoader::CreateTerrain(int width, int height, HeightMapType* m_heightMap, float scale, MeshPattern meshPattern)
{
    MeshObject mesh;

    mesh.scale = scale;

    // Calculate the number of vertices in the terrain mesh.
    mesh.vertexCount = (width - 1) * (height - 1) * 6;

    // Set the index count to the same as the vertex count.
    mesh.indexCount = mesh.vertexCount;

    int indexCount = 0; // index counter 
    int index1, index2, index3, index4; //geometric indices.

    bool diagRight = true;

    for (int j = 0; j < (width - 1); j++)
    {
        for (int i = 0; i < (height - 1); i++)
        {
            index1 = (height * j) + i;          // Bottom left.
            index2 = (height * j) + (i + 1);      // Bottom right.
            index3 = (height * (j + 1)) + i;      // Upper left.
            index4 = (height * (j + 1)) + (i + 1);  // Upper right.

            diagRight = !diagRight;

            if (diagRight) {
                // Upper left.
                AddVertex(m_heightMap, index3, mesh, indexCount);

                // Upper right.
                AddVertex(m_heightMap, index4, mesh, indexCount);

                // Bottom left.
                AddVertex(m_heightMap, index1, mesh, indexCount);

                // Bottom left.
                AddVertex(m_heightMap, index1, mesh, indexCount);

                // Upper right.
                AddVertex(m_heightMap, index4, mesh, indexCount);

                // Bottom right.
                AddVertex(m_heightMap, index2, mesh, indexCount);
            }
            else
            {
                // Upper right.
                AddVertex(m_heightMap, index4, mesh, indexCount);

                // Bottom right.
                AddVertex(m_heightMap, index2, mesh, indexCount);

                // Upper left.
                AddVertex(m_heightMap, index3, mesh, indexCount);

                // Upper left.
                AddVertex(m_heightMap, index3, mesh, indexCount);

                // Bottom right.
                AddVertex(m_heightMap, index2, mesh, indexCount);

                // Bottom left.
                AddVertex(m_heightMap, index1, mesh, indexCount);
            }
        }
    }

    return mesh;
}

void ModelLoader::AddVertex(HeightMapType * m_heightMap, int index3, MeshObject &mesh, int &indexCount)
{
    AddTerrainVertex(m_heightMap, index3, mesh);
    mesh.indices.push_back(indexCount);
    indexCount++;
}

void ModelLoader::AddTerrainVertex(HeightMapType * m_heightMap, int geometricIndex, MeshObject &mesh)
{
    VertexPositionNormalTexture vertex;
    vertex.position = DirectX::SimpleMath::Vector3(m_heightMap[geometricIndex].x, m_heightMap[geometricIndex].y, m_heightMap[geometricIndex].z);
    vertex.normal = DirectX::SimpleMath::Vector3(m_heightMap[geometricIndex].nx, m_heightMap[geometricIndex].ny, m_heightMap[geometricIndex].nz);
    vertex.textureCoordinate = DirectX::SimpleMath::Vector2(m_heightMap[geometricIndex].u, m_heightMap[geometricIndex].v);
    mesh.verticies.push_back(vertex);
}

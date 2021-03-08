////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "modelclass.h"
#include "ModelLoader.h"

using namespace DirectX;

ModelClass::ModelClass()
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::InitializeModel(DX::DeviceResources& deviceResources, char* filename)
{
	LoadModel(filename);
	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(deviceResources);
	if (!result)
	{
		return false;
	}
	return true;
}


bool ModelClass::InitializeSphere(DX::DeviceResources& deviceResources)
{
    std::vector<uint16_t> indicies;

    GeometricPrimitive::CreateSphere(preFabVertices, indicies, 1, 8, false);

    preFabIndices.assign(indicies.begin(), indicies.end());

	m_vertexCount = preFabVertices.size();
	m_indexCount = preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(deviceResources);
	if (!result)
	{
		return false;
	}
	return true;
}

bool ModelClass::InitializeBox(DX::DeviceResources& deviceResources, float xwidth, float yheight, float zdepth)
{
    std::vector<uint16_t> indicies;

    GeometricPrimitive::CreateBox(preFabVertices, indicies,
        DirectX::SimpleMath::Vector3(xwidth, yheight, zdepth), false);

    preFabIndices.assign(indicies.begin(), indicies.end());

	m_vertexCount = preFabVertices.size();
	m_indexCount = preFabIndices.size();

	bool result;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers(deviceResources);
	if (!result)
	{
		return false;
	}
	return true;
}

void ModelClass::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}

void ModelClass::Render(DX::DeviceResources& deviceResources)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceResources);
	deviceResources.GetD3DDeviceContext()->DrawIndexed(m_indexCount, 0, 0);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


bool ModelClass::InitializeBuffers(DX::DeviceResources& deviceResources)
{
	m_vertexBuffer1 = std::make_unique<VertexBuffer>(deviceResources, preFabVertices);
	m_indexBuffer1 = std::make_unique<IndexBuffer>(deviceResources, preFabIndices);
	return true;
}


void ModelClass::ShutdownBuffers()
{
	return;
}


void ModelClass::RenderBuffers(DX::DeviceResources& deviceResources)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	 m_vertexBuffer1->Bind(deviceResources);

    // Set the index buffer to active in the input assembler so it can be rendered.
	 m_indexBuffer1->Bind(deviceResources);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	// 
	deviceResources.GetD3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadModel(char* filename)
{
	ModelLoader modelLoader;

	auto model = modelLoader.LoadModel(filename);

	preFabIndices = model.indices;
	preFabVertices = model.verticies;
	m_indexCount = model.indexCount;
	m_vertexCount = model.vertexCount;

	return true;
}

void ModelClass::ReleaseModel()
{
	return;
}
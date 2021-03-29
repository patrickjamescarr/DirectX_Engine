////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "pch.h"
#include "Light.h"
#include <fstream>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "DeviceResources.h"

//#include <d3dx10math.h>
//#include <fstream>
//using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

using namespace DirectX;

class ModelClass
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	~ModelClass(); 

	bool InitializeModel(DX::DeviceResources& deviceResources, char* filename);
	bool InitializeSphere(DX::DeviceResources& deviceResources);
	bool InitializeBox(DX::DeviceResources& deviceResources, float xwidth, float yheight, float zdepth);
	void Shutdown();
	void Render(DX::DeviceResources& deviceResources);
	int GetIndexCount();


private:
	bool InitializeBuffers(DX::DeviceResources& deviceResources);
	void ShutdownBuffers();
	void RenderBuffers(DX::DeviceResources& deviceResources);
	bool LoadModel(char*);

	void ReleaseModel();

private:
	int m_vertexCount, m_indexCount;

	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint32_t> preFabIndices;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
	std::unique_ptr<DirectX::Model> m_model;
	std::unique_ptr<ModelType[]> m_modelTxt;

	std::unique_ptr<VertexBuffer<DirectX::VertexPositionNormalTexture>> m_vertexBuffer1;
	std::unique_ptr<IndexBuffer> m_indexBuffer1;

};

#endif
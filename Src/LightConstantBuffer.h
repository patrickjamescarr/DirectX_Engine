#pragma once
#include "Bindable.h"
#include "PixelConstantBuffer.h"
#include "GameObject.h"
#include "DeviceResources.h"
#include "Light.h"

class LightConstantBuffer : public Bindable
{
private :
	//buffer for information of a single light
	struct LightBufferType
	{
		DirectX::SimpleMath::Vector4 ambient;
		DirectX::SimpleMath::Vector4 diffuse;
		DirectX::SimpleMath::Vector3 position;
		float padding;
	};
public:
	LightConstantBuffer(DX::DeviceResources& deviceResources,  Light *sceneLight);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
private :
	std::unique_ptr<PixelConstantBuffer<LightBufferType>> m_lightBuffer;
	Light* m_sceneLight;
};


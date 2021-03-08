#pragma once
#include "DeviceResources.h"
#include "GameObjectBase.h"

class SkyBox : public GameObject
{
public:
	SkyBox(DX::DeviceResources& deviceResources);
	void Draw(DX::DeviceResources& deviceResources) const;
	virtual DirectX::SimpleMath::Matrix GetTransform() const noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	UINT m_cubeIndexCount;
};


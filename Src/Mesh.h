#pragma once
#include "GameObjectBase.h"
#include "Shader.h"

class Mesh : public GameObject
{
public:
	Mesh(DX::DeviceResources& deviceResources, std::vector<std::unique_ptr<Bindable>> bindables, Light* light, const wchar_t* textureFileName, DirectX::SimpleMath::Matrix transform);
    void AddDefaultBindables(DX::DeviceResources & deviceResources, Light * &light, const wchar_t * &textureFileName);
    Mesh(DX::DeviceResources& deviceResources, Light* light, const wchar_t* textureFileName, DirectX::SimpleMath::Matrix transform);
	virtual void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const;
	virtual DirectX::SimpleMath::Matrix GetTransform() const noexcept override;
    virtual void Update() {};
    void SetTransform(DirectX::SimpleMath::Matrix transform) noexcept;
protected:
	void AddBindables(std::vector<std::unique_ptr<Bindable>>& bindables);
private:
	mutable DirectX::SimpleMath::Matrix m_transform;
    mutable DirectX::SimpleMath::Matrix m_accumulatedTransform;
};



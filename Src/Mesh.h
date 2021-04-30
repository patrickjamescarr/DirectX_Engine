#pragma once
#include "GameObjectBase.h"
#include "Shader.h"

class Mesh : public GameObject
{
public:
    Mesh(DirectX::SimpleMath::Matrix transform);
	Mesh(
        DX::DeviceResources& deviceResources, 
        std::vector<std::unique_ptr<Bindable>> bindables, 
        Light* light, 
        const wchar_t* textureFileName, 
        DirectX::SimpleMath::Matrix transform,
        D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    Mesh(DX::DeviceResources& deviceResources,
        Light * sceneLight,
        DirectX::SimpleMath::Matrix transform,
        const wchar_t * textureFileName,
        const wchar_t * vertexShaderFileName,
        const wchar_t * pixelShaderFileName,
        D3D11_CULL_MODE cullMode = D3D11_CULL_BACK
    );

    void AddDefaultBindables(DX::DeviceResources & deviceResources, Light * &light, const wchar_t * &textureFileName, D3D_PRIMITIVE_TOPOLOGY topology);
    Mesh(DX::DeviceResources& deviceResources, Light* light, const wchar_t* textureFileName, DirectX::SimpleMath::Matrix transform);
	virtual void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const;
	virtual DirectX::SimpleMath::Matrix GetTransform() const noexcept override;
    virtual void Update() {};
    void SetTransform(DirectX::SimpleMath::Matrix transform) noexcept;
protected:
	void AddBindables(std::vector<std::unique_ptr<Bindable>>& bindables);
protected:
    mutable DirectX::SimpleMath::Matrix m_accumulatedTransform;
private:
	mutable DirectX::SimpleMath::Matrix m_transform;
};



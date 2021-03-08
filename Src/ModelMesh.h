#pragma once

#include "GameObject.h"
#include "Shader.h"

class ModelMesh : GameObject
{
	ModelMesh();
	void Draw(ID3D11DeviceContext* deviceContext, DirectX::FXMMATRIX parentTransform) const;
	DirectX::XMMATRIX GetTransform() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&transform);
	}
private:
	mutable DirectX::XMFLOAT4X4 transform;

};


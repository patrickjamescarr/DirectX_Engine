#pragma once
#include "Mesh.h"

class BoxCollider : public Mesh
{
public:
    BoxCollider(DX::DeviceResources& deviceResources, DirectX::SimpleMath::Matrix transform, DirectX::SimpleMath::Vector3 dimentions, float scale = 1);

    void Draw(DX::DeviceResources& deviceResources, DirectX::FXMMATRIX accumulatedTransform) const override;

    bool DetectCollision(const BoxCollider& other);

public:

    float halfWidth;
    float halfHeight;
};


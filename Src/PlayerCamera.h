#pragma once
#include "Camera.h"
#include "Collision.h"

class PlayerCamera :
    public Camera
{
public:
    void adjustHeight(float height);
    void setHeight(float height);
    void setPosition(DirectX::SimpleMath::Vector3 newPosition) override;;
    void setCollision(Collision* collision) { m_collision = collision; }
    void setDensity(float density) { m_density = density; }
    void setDensities1(DirectX::SimpleMath::Vector4 densities) { m_densities_1 = densities; }
    void setDensities2(DirectX::SimpleMath::Vector4 densities) { m_densities_2 = densities; }
    void initPosition(DirectX::SimpleMath::Vector3 position) { m_position = position; }
private:
    Collision* m_collision;
    float m_density;
    DirectX::SimpleMath::Vector4 m_densities_1;
    DirectX::SimpleMath::Vector4 m_densities_2;
    
};
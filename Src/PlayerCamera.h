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
    void setDensities3(DirectX::SimpleMath::Vector4 densities) { m_densities_3 = densities; }
    void setDensities4(DirectX::SimpleMath::Vector4 densities) { m_densities_4 = densities; }
    void setDensities5(DirectX::SimpleMath::Vector4 densities) { m_densities_5 = densities; }
    void setDensities6(DirectX::SimpleMath::Vector4 densities) { m_densities_6 = densities; }
    void setDensities7(DirectX::SimpleMath::Vector4 densities) { m_densities_7 = densities; }

    void initPosition(DirectX::SimpleMath::Vector3 position) { m_position = position; }
    void chestFound() { m_chestsFound++; };
    int  treasureChestsFound() { return m_chestsFound; }
private:
    Collision* m_collision;
    float m_density;
    DirectX::SimpleMath::Vector4 m_densities_1;
    DirectX::SimpleMath::Vector4 m_densities_2;
    DirectX::SimpleMath::Vector4 m_densities_3;
    DirectX::SimpleMath::Vector4 m_densities_4;
    DirectX::SimpleMath::Vector4 m_densities_5;
    DirectX::SimpleMath::Vector4 m_densities_6;
    DirectX::SimpleMath::Vector4 m_densities_7;

    int m_chestsFound = 0;
};


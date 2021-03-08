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
    void initPosition(DirectX::SimpleMath::Vector3 position) { m_position = position; }
private:
    Collision* m_collision;
};
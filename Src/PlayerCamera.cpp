#include "pch.h"
#include "PlayerCamera.h"

void PlayerCamera::adjustHeight(float height)
{
    auto newHeight = m_position.y - (height - 1);
    m_position.y = std::max(-1.0f, newHeight);
}

void PlayerCamera::setHeight(float height)
{
    m_position.y = height;
}

void PlayerCamera::setPosition(DirectX::SimpleMath::Vector3 newPosition)
{
    if (m_collision == nullptr)
    {
        m_position.x = newPosition.x;
        m_position.y = newPosition.y;
        m_position.z = newPosition.z;

        return;
    }

    bool movingRight = (newPosition.x - m_position.x) > 0;
    bool movingLeft = (newPosition.x - m_position.x) < 0;
    bool movingForward = (newPosition.z - m_position.z) > 0;
    bool movingBack = (newPosition.z - m_position.z) < 0;


    bool canMoveX = false;
    bool canMoveZ = false;

    if (movingBack)
    {
        canMoveZ = m_collision->canMoveBack();
    }

    if (movingForward)
    {
        canMoveZ = m_collision->canMoveForward();
    }

    if (movingLeft)
    {
        canMoveX = m_collision->canMoveLeft();
    }

    if (movingRight)
    {
        canMoveX = m_collision->canMoveRight();
    }

    if(canMoveX)
        m_position.x = newPosition.x;

    if (canMoveZ)
        m_position.z = newPosition.z;
}

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
    //if (m_density < 0)
    //{
    //    m_position.x = newPosition.x;
    //    m_position.y = newPosition.y;
    //    m_position.z = newPosition.z;
    //    return;
    //}
    //else
    //{
    //    return;
    //}

    //if (m_collision == nullptr)
    //{
    //    m_position.x = newPosition.x;
    //    m_position.y = newPosition.y;
    //    m_position.z = newPosition.z;

    //    return;
    //}

    bool movingRight = (newPosition.x - m_position.x) > 0;
    bool movingLeft = (newPosition.x - m_position.x) < 0;
    bool movingForward = (newPosition.z - m_position.z) > 0;
    bool movingBack = (newPosition.z - m_position.z) < 0;
    bool movingUp = (newPosition.y - m_position.y) > 0;
    bool movingDown = (newPosition.y - m_position.y) < 0;


    bool canMoveX = false;
    bool canMoveY = false;
    bool canMoveZ = false;

    if (movingForward && movingUp && movingRight)
    {
        canMoveX = m_densities_3.z < 0;
        canMoveZ = m_densities_3.z < 0;
        canMoveY = m_densities_3.z < 0;
    }
    else if (movingBack && movingUp && movingRight)
    {
        canMoveX = m_densities_4.x < 0;
        canMoveZ = m_densities_4.x < 0;
        canMoveY = m_densities_4.x < 0;
    }
    else if (movingBack && movingUp && movingLeft)
    {
        canMoveX = m_densities_4.z < 0;
        canMoveY = m_densities_4.z < 0;
        canMoveZ = m_densities_4.z < 0;
    }
    else if (movingUp && movingLeft && movingForward)
    {
        canMoveX = m_densities_5.x < 0;
        canMoveY = m_densities_5.x < 0;
        canMoveZ = m_densities_5.x < 0;
    }

    else if (movingForward && movingDown && movingRight)
    {
        canMoveX = m_densities_5.z < 0;
        canMoveZ = m_densities_5.z < 0;
        canMoveY = m_densities_5.z < 0;
    }
    else if (movingBack && movingDown && movingRight)
    {
        canMoveX = m_densities_6.x < 0;
        canMoveZ = m_densities_6.x < 0;
        canMoveY = m_densities_6.x < 0;
    }
    else if (movingBack && movingDown && movingLeft)
    {
        canMoveX = m_densities_6.z < 0;
        canMoveY = m_densities_6.z < 0;
        canMoveZ = m_densities_6.z < 0;
    }
    else if (movingDown && movingLeft && movingForward)
    {
        canMoveX = m_densities_7.x < 0;
        canMoveY = m_densities_7.x < 0;
        canMoveZ = m_densities_7.x < 0;
    }


    else if (movingForward && movingUp)
    {
        canMoveZ = m_densities_3.y < 0;
        canMoveY = m_densities_3.y < 0;
    }
    else if (movingBack && movingUp)
    {
        canMoveZ = m_densities_4.y < 0;
        canMoveY = m_densities_4.y < 0;
    }
    else if (movingUp && movingRight)
    {
        canMoveX = m_densities_3.w < 0;
        canMoveY = m_densities_3.w < 0;
    }
    else if (movingUp && movingLeft)
    {
        canMoveX = m_densities_4.w < 0;
        canMoveY = m_densities_4.w < 0;
    }

    else if (movingForward && movingDown)
    {
        canMoveZ = m_densities_5.y < 0;
        canMoveY = m_densities_5.y < 0;
    }
    else if (movingBack && movingDown)
    {
        canMoveZ = m_densities_6.y < 0;
        canMoveY = m_densities_6.y < 0;
    }
    else if (movingDown && movingRight)
    {
        canMoveX = m_densities_5.w < 0;
        canMoveY = m_densities_5.w < 0;
    }
    else if (movingDown && movingLeft)
    {
        canMoveX = m_densities_6.w < 0;
        canMoveY = m_densities_6.w < 0;
    }

    else if (movingForward && movingLeft)
    {
        canMoveZ = m_densities_1.y < 0;
        canMoveX = m_densities_1.y < 0;
    }
    else if (movingBack && movingLeft)
    {
        canMoveZ = m_densities_1.w < 0;
        canMoveX = m_densities_1.w < 0;
    }
    else if (movingBack && movingRight)
    {
        canMoveZ = m_densities_2.y < 0;
        canMoveX = m_densities_2.y < 0;
    }
    else if (movingForward && movingRight)
    {
        canMoveZ = m_densities_2.w < 0;
        canMoveX = m_densities_2.w < 0;
    }

    else if (movingForward)
    {
        canMoveZ = m_densities_1.x < 0;
    }
    else if (movingLeft)
    {
        canMoveX = m_densities_1.z < 0;
    }
    else if (movingBack)
    {
        canMoveZ = m_densities_2.x < 0;
    }
    else if (movingRight)
    {
        canMoveX = m_densities_2.z < 0;
    }
    else if (movingUp)
    {
        canMoveY = m_densities_3.x < 0;
    }
    else if (movingDown)
    {
        canMoveY = m_densities_7.y < 0;
    }

    if(canMoveX && newPosition.x > 0.0f && newPosition.x < m_boundry.x)
        m_position.x = newPosition.x;

    if (canMoveY && newPosition.y > 0.5f && newPosition.y < m_boundry.y)
        m_position.y = newPosition.y;

    if (canMoveZ && newPosition.z > 0.0f && newPosition.z < m_boundry.z)
        m_position.z = newPosition.z;
}

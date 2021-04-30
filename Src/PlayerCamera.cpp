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


    bool canMoveX = false;
    bool canMoveZ = false;

    if (movingForward && movingLeft)
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


    //if (movingBack)
    //{
    //    canMoveZ = m_densities_2.x < 1;
    //        //&&  // back
    //        //m_densities_2.y < 0 &&  // back right 
    //        //m_densities_1.w < 0;    // back left

    //    if (movingRight)
    //    {
    //        canMoveZ = canMoveZ && m_densities_2.y < 1;
    //    }

    //    if (movingLeft)
    //    {
    //        canMoveZ = canMoveZ && m_densities_1.w < 1;
    //    }
    //}

    //if (movingForward)
    //{
    //    canMoveZ = m_densities_1.x < 1; // && m_densities_1.y < 0 && m_densities_2.w < 0;

    //    if (movingRight)
    //    {
    //        canMoveZ = canMoveZ && m_densities_2.w < 1;
    //    }

    //    if (movingLeft)
    //    {
    //        canMoveZ = canMoveZ && m_densities_1.y < 1;
    //    }
    //}

    //if (movingLeft)
    //{
    //    canMoveX = m_densities_1.z < 1;

    //    if (movingBack)
    //    {
    //        canMoveX = canMoveX && m_densities_1.w < 1;
    //    }

    //    if (movingForward)
    //    {
    //        canMoveX = canMoveX && m_densities_1.y < 1;
    //    }
    //}

    //if (movingRight)
    //{
    //    canMoveX = m_densities_2.z < 1;

    //    if (movingBack)
    //    {
    //        canMoveX = canMoveX && m_densities_2.y < 1;
    //    }

    //    if (movingForward)
    //    {
    //        canMoveX = canMoveX && m_densities_2.w < 1;
    //    }
    //}

    if(canMoveX)
        m_position.x = newPosition.x;

    if (canMoveZ)
        m_position.z = newPosition.z;
}

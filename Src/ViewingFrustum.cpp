#include "pch.h"
#include "ViewingFrustum.h"

using namespace DirectX::SimpleMath;

ViewingFrustum::ViewingFrustum()
{
}

ViewingFrustum::~ViewingFrustum()
{
}

void ViewingFrustum::ConstructFrustum(float screenDepth, DirectX::SimpleMath::Matrix projection, DirectX::SimpleMath::Matrix view)
{
    // Calculate the minimum Z distance in the frustum.
    float zMinimum = -projection._43 / projection._33;
    float r = screenDepth / (screenDepth - zMinimum);
    projection._33 = r;
    projection._43 = -r * zMinimum;

    // Create the frustum matrix from the view matrix and updated projection matrix.
    Matrix matrix = view * projection;

    // Calculate near plane of frustum.
    m_planes[0].x = matrix._14 + matrix._13;
    m_planes[0].y = matrix._24 + matrix._23;
    m_planes[0].x = matrix._34 + matrix._33;
    m_planes[0].w = matrix._44 + matrix._43;
    m_planes[0].Normalize();

    // Calculate far plane of frustum.
    m_planes[1].x = matrix._14 - matrix._13;
    m_planes[1].y = matrix._24 - matrix._23;
    m_planes[1].z = matrix._34 - matrix._33;
    m_planes[1].w = matrix._44 - matrix._43;
    m_planes[1].Normalize();

    // Calculate left plane of frustum.
    m_planes[2].x = matrix._14 + matrix._11;
    m_planes[2].y = matrix._24 + matrix._21;
    m_planes[2].z = matrix._34 + matrix._31;
    m_planes[2].w = matrix._44 + matrix._41;
    m_planes[2].Normalize();

    // Calculate right plane of frustum.
    m_planes[3].x = matrix._14 - matrix._11;
    m_planes[3].y = matrix._24 - matrix._21;
    m_planes[3].z = matrix._34 - matrix._31;
    m_planes[3].w = matrix._44 - matrix._41;
    m_planes[3].Normalize();

    // Calculate top plane of frustum.
    m_planes[4].x = matrix._14 - matrix._12;
    m_planes[4].y = matrix._24 - matrix._22;
    m_planes[4].z = matrix._34 - matrix._32;
    m_planes[4].w = matrix._44 - matrix._42;
    m_planes[4].Normalize();

    // Calculate bottom plane of frustum.
    m_planes[5].x = matrix._14 + matrix._12;
    m_planes[5].y = matrix._24 + matrix._22;
    m_planes[5].z = matrix._34 + matrix._32;
    m_planes[5].w = matrix._44 + matrix._42;
    m_planes[5].Normalize();

    return;
}

bool ViewingFrustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
    int i;

    // Check if any one point of the cube is in the view frustum.
    for (i = 0; i < 6; i++)
    {
        if (m_planes[i].DotCoordinate(Vector3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
        {
            continue;
        }

        if (m_planes[i].DotCoordinate(Vector3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
        {
            continue;
        }

        if (m_planes[i].DotCoordinate(Vector3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
        {
            continue;
        }

        if (m_planes[i].DotCoordinate(Vector3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
        {
            continue;
        }

        if (m_planes[i].DotCoordinate(Vector3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
        {
            continue;
        }

        if (m_planes[i].DotCoordinate(Vector3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
        {
            continue;
        }

        if (m_planes[i].DotCoordinate(Vector3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
        {
            continue;
        }

        if (m_planes[i].DotCoordinate(Vector3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
        {
            continue;
        }

        return false;
    }

    return true;
}

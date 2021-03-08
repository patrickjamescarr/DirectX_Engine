#include "pch.h"
#include "CollisionDetection.h"

using namespace DirectX::SimpleMath;

#define kEpsilon 1e-8f

CollisionDetection::CollisionDetection()
{
}

bool CollisionDetection::rayTriangleIntersect(
    const Vector3 & rayOrigin, 
    const Vector3 & rayDirection, 
    const Vector3 & v0, 
    const Vector3 & v1, 
    const Vector3 & v2, 
    float & t,
    bool infiniteRay
) const
{
    bool br = false;

    if (v0.x == 1.0f && v0.z == 1.00f &&
        v1.x == 2.0f && v1.z == 2.00f &&
        v2.x == 2.0f && v2.z == 1.00f)
    {
        br = true;
    }

    // compute plane's normal
    Vector3 v0v1 = v1 - v0;
    Vector3 v0v2 = v2 - v0;
    // no need to normalize
    Vector3 N = v0v1.Cross(v0v2); // N 
    float area2 = N.Length();

    // Step 1: finding P

    // check if ray and plane are parallel ?
    float NdotRayDirection = N.Dot(rayDirection);

    if (fabs(NdotRayDirection) < kEpsilon) // almost 0 
    {
        return false; // they are parallel so they don't intersect ! 
    }

    // compute d parameter using equation 2
    float d = N.Dot(v0);

    // compute t (equation 3)
    t = (N.Dot(rayOrigin) + d) / NdotRayDirection;

    // check if the triangle is in behind the ray
    // and if the ray is not infinite, out of the rays reach
    if (t < 0 || (!infiniteRay && t > 1))
    {
        return false; // the triangle is behind 
    }

    // compute the intersection point using equation 1
    Vector3 P = rayOrigin + t * rayDirection;

    // Step 2: inside-outside test
    Vector3 C; // vector perpendicular to triangle's plane 

    // edge 0
    Vector3 edge0 = v1 - v0;
    Vector3 vp0 = P - v0;
    C = edge0.Cross(vp0);

    if (N.Dot(C) < 0) 
    {
        return false; // P is on the right side 
    }

    // edge 1
    Vector3 edge1 = v2 - v1;
    Vector3 vp1 = P - v1;
    C = edge1.Cross(vp1);

    if (N.Dot(C) < 0) 
    {
        return false; // P is on the right side 
    }

    // edge 2
    Vector3 edge2 = v0 - v2;
    Vector3 vp2 = P - v2;
    C = edge2.Cross(vp2);

    if (N.Dot(C) < 0) 
    {
        return false; // P is on the right side 
    }

    if (ImGui::Begin("Collision output"))
    {
        ImGui::Text("Vector collision");
        ImGui::Text("V1 = %f, %f, %f", v0.x, v0.y, v0.z);
        ImGui::Text("V2 = %f, %f, %f", v1.x, v1.y, v1.z);
        ImGui::Text("V3 = %f, %f, %f", v2.x, v2.y, v2.z);
        ImGui::Text("t = %f", t);
    }

    ImGui::End();

    return true; // this ray hits the triangle 
}

bool CollisionDetection::rayTriangleIntersectMT(
    const DirectX::SimpleMath::Vector3 & orig,
    const DirectX::SimpleMath::Vector3 & dir,
    const DirectX::SimpleMath::Vector3 & v0,
    const DirectX::SimpleMath::Vector3 & v1,
    const DirectX::SimpleMath::Vector3 & v2,
    float & t,
    float & u,
    float & v,
    bool culling
) const
{
    DirectX::SimpleMath::Vector3 v0v1 = v1 - v0;
    DirectX::SimpleMath::Vector3 v0v2 = v2 - v0;
    DirectX::SimpleMath::Vector3 pvec = dir.Cross(v0v2);
    float det = v0v1.Dot(pvec);

    if (culling)
    {
        // if the determinant is negative the triangle is backfacing
        // if the determinant is close to 0, the ray misses the triangle
        if (det < kEpsilon) 
        {
            return false;
        }
    }
    else
    {
        // ray and triangle are parallel if det is close to 0
        if (fabs(det) < kEpsilon)
        {
            return false;
        }
    }

    float invDet = 1 / det;

    DirectX::SimpleMath::Vector3 tvec = orig - v0;

    u = tvec.Dot(pvec) * invDet;

    if (u < 0 || u > 1)
    {
        return false;
    }

    DirectX::SimpleMath::Vector3 qvec = tvec.Cross(v0v1);

    v = dir.Dot(qvec) * invDet;

    if (v < 0 || u + v > 1)
    {
        return false;
    }

    t = v0v2.Dot(qvec) * invDet;

    if (ImGui::Begin("Collision output"))
    {
        ImGui::Text("Vector collision");
        ImGui::Text("V1 = %f, %f, %f", v0.x, v0.y, v0.z);
        ImGui::Text("V2 = %f, %f, %f", v1.x, v1.y, v1.z);
        ImGui::Text("V3 = %f, %f, %f", v2.x, v2.y, v2.z);
        ImGui::Text("t = %f", t);
    }

    ImGui::End();

    return true;
}

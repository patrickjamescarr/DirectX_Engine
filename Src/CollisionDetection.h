#pragma once
class CollisionDetection
{
public:
    CollisionDetection();

    bool rayTriangleIntersect(
        const DirectX::SimpleMath::Vector3 &rayOrigin, const DirectX::SimpleMath::Vector3 &rayDirection,
        const DirectX::SimpleMath::Vector3 &v0, const DirectX::SimpleMath::Vector3 &v1, const DirectX::SimpleMath::Vector3 &v2,
        float &t, bool infiniteRay = true) const;

    bool rayTriangleIntersectMT(
        const DirectX::SimpleMath::Vector3 & orig,
        const DirectX::SimpleMath::Vector3 & dir,
        const DirectX::SimpleMath::Vector3 & v0,
        const DirectX::SimpleMath::Vector3 & v1,
        const DirectX::SimpleMath::Vector3 & v2,
        float & t,
        float & u,
        float & v,
        bool culling = false) const;
};


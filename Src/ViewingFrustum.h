#pragma once
class ViewingFrustum
{
public:
    ViewingFrustum();
    ~ViewingFrustum();

   void ConstructFrustum(float screenDepth, DirectX::SimpleMath::Matrix projection, DirectX::SimpleMath::Matrix view);

    bool CheckCube(float, float, float, float);

private:
    DirectX::SimpleMath::Plane m_planes[6];
};


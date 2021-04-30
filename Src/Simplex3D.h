#pragma once
class Simplex3D
{
public:
    Simplex3D();

private:
    DirectX::SimpleMath::Vector3 mod289(DirectX::SimpleMath::Vector3 x);

    DirectX::SimpleMath::Vector4 mod289(DirectX::SimpleMath::Vector4 x);

    DirectX::SimpleMath::Vector4 permute(DirectX::SimpleMath::Vector4 x);

    DirectX::SimpleMath::Vector4 taylorInvSqrt(DirectX::SimpleMath::Vector4 r);

    DirectX::SimpleMath::Vector3 floor(DirectX::SimpleMath::Vector3 v)
    {
        DirectX::SimpleMath::Vector3 rv;
        float i;

        rv.x = v.x - std::modf(v.x, &i);
        rv.y = v.y - std::modf(v.y, &i);
        rv.z = v.z - std::modf(v.z, &i);
        return rv;
    }
    DirectX::SimpleMath::Vector4 floor(DirectX::SimpleMath::Vector4 v)
    {
        DirectX::SimpleMath::Vector4 rv;
        float i;

        rv.x = v.x - std::modf(v.x, &i);
        rv.y = v.y - std::modf(v.y, &i);
        rv.z = v.z - std::modf(v.z, &i);
        rv.w = v.w - std::modf(v.w, &i);
        return rv;
    }

    DirectX::SimpleMath::Vector3  step(DirectX::SimpleMath::Vector3  a, DirectX::SimpleMath::Vector3  x)
    {
        DirectX::SimpleMath::Vector3 step;
        step.x = x.x >= a.x ? 1.0f : 0.0f;
        step.y = x.y >= a.y ? 1.0f : 0.0f;
        step.z = x.z >= a.z ? 1.0f : 0.0f;
        return step;
    }

    DirectX::SimpleMath::Vector4 step(DirectX::SimpleMath::Vector4  a, DirectX::SimpleMath::Vector4  x)
    {
        DirectX::SimpleMath::Vector4 step;
        step.x = x.x >= a.x ? 1.0f : 0.0f;
        step.y = x.y >= a.y ? 1.0f : 0.0f;
        step.z = x.z >= a.z ? 1.0f : 0.0f;
        step.w = x.w >= a.w ? 1.0f : 0.0f;
        return step;
    }
public:

    float snoise(DirectX::SimpleMath::Vector3 v);

};


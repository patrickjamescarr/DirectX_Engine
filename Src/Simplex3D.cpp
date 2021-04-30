#include "pch.h"
#include "Simplex3D.h"
using namespace DirectX::SimpleMath;
Simplex3D::Simplex3D()
{
}

DirectX::SimpleMath::Vector3 Simplex3D::mod289(DirectX::SimpleMath::Vector3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

DirectX::SimpleMath::Vector4 Simplex3D::mod289(DirectX::SimpleMath::Vector4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

DirectX::SimpleMath::Vector4 Simplex3D::permute(DirectX::SimpleMath::Vector4 x)
{
    return mod289(((x*34.0) + Vector4::One)*x);
}

DirectX::SimpleMath::Vector4 Simplex3D::taylorInvSqrt(DirectX::SimpleMath::Vector4 r)
{
    float val = 1.79284291400159f;
    return DirectX::SimpleMath::Vector4(val, val, val, val) - 0.85373472095314 * r;
}

float Simplex3D::snoise(DirectX::SimpleMath::Vector3 v)
{
    const Vector2  C = Vector2(1.0 / 6.0, 1.0 / 3.0);
    const Vector4  D = Vector4(0.0, 0.5, 1.0, 2.0);

    auto vDotCy = v.Dot(Vector3(C.y, C.y, C.y));


    // First corner
    Vector3 i = floor(v + Vector3(vDotCy, vDotCy, vDotCy));

    auto iDotCx = i.Dot(Vector3(C.x, C.x, C.x));

    Vector3 x0 = v - i + Vector3(iDotCx, iDotCx, iDotCx);

    

    // Other corners
    Vector3 g = step(Vector3(x0.y, x0.z, x0.x), Vector3(x0.x, x0.y, x0.z));
    Vector3 l = Vector3::One - g;
    Vector3 i1 = Vector3::Min(g, Vector3(l.z, l.x, l.y));
    Vector3 i2 = Vector3::Max(g, Vector3(l.z, l.x, l.y));

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    Vector3 x1 = x0 - i1 + Vector3(C.x, C.x, C.x);
    Vector3 x2 = x0 - i2 + Vector3(C.y, C.y, C.y); // 2.0*C.x = 1/3 = C.y
    Vector3 x3 = x0 - Vector3(D.y, D.y, D.y);      // -1.0+3.0*C.x = -0.5 = -D.y

  // Permutations
    i = mod289(i);
    Vector4 p = permute(permute(permute(
        Vector4(i.z, i.z, i.z, i.z) + Vector4(0.0, i1.z, i2.z, 1.0))
        + Vector4(i.y, i.y, i.y, i.y) + Vector4(0.0, i1.y, i2.y, 1.0))
        + Vector4(i.x, i.x, i.x, i.x) + Vector4(0.0, i1.x, i2.x, 1.0));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    Vector3  ns = n_ * Vector3(D.w, D.y, D.z) - Vector3(D.x, D.z, D.x);

    Vector4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    Vector4 x_ = floor(j * ns.z);
    Vector4 y_ = floor(j - 7.0 * x_);    // mod(j,N)

    Vector4 x = x_ * ns.x + Vector4(ns.y, ns.y, ns.y, ns.y);
    Vector4 y = y_ * ns.x + Vector4(ns.y, ns.y, ns.y, ns.y);
    Vector4 h = Vector4::One - Vector4(std::abs(x.x), std::abs(x.y), std::abs(x.z), std::abs(x.w)) - Vector4(std::abs(y.x), std::abs(y.y), std::abs(y.z), std::abs(y.w));

    Vector4 b0 = Vector4(x.x, x.y, y.x, y.y);
    Vector4 b1 = Vector4(x.z, x.w, y.z, y.w);

    //Vector4 s0 = Vector4(lessThan(b0,0.0))*2.0 - 1.0;
    //Vector4 s1 = Vector4(lessThan(b1,0.0))*2.0 - 1.0;
    Vector4 s0 = floor(b0)*2.0 + Vector4::One;
    Vector4 s1 = floor(b1)*2.0 + Vector4::One;
    Vector4 sh = -step(h, Vector4(0, 0, 0, 0));

    Vector4 a0 = Vector4(b0.x, b0.z, b0.y, b0.w) + Vector4(s0.x, s0.z, s0.y, s0.w)*Vector4(sh.x, sh.x, sh.y, sh.y);
    Vector4 a1 = Vector4(b1.x, b1.z, b1.y, b1.w) + Vector4(s1.x, s1.z, s1.y, s1.w)*Vector4(sh.z, sh.z, sh.w, sh.w);

    Vector3 p0 = Vector3(a0.x, a0.y, h.x);
    Vector3 p1 = Vector3(a0.z, a0.w, h.y);
    Vector3 p2 = Vector3(a1.x, a1.y, h.z);
    Vector3 p3 = Vector3(a1.z, a1.w, h.w);

    //Normalise gradients
    Vector4 norm = taylorInvSqrt(Vector4(p0.Dot(p0), p1.Dot(p1), p2.Dot(p2), p3.Dot(p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    Vector4 m = Vector4::Max(Vector4(0.5, 0.5, 0.5, 0.5) - Vector4(x0.Dot(x0), x1.Dot(x1), x2.Dot(x2), x3.Dot(x3)), Vector4::Zero);
    m = m * m;
    return 105.0 * (m*m).Dot( Vector4(p0.Dot( x0), p1.Dot(x1),
        p2.Dot(x2), p3.Dot(x3)));
}

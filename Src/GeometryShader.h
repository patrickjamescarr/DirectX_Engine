#pragma once
#include "Bindable.h"

class GeometryShader :
    public Bindable
{
public:
    GeometryShader(DX::DeviceResources& deviceResources, const WCHAR * fileName, bool streamOutput = false);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
protected:
    Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
};


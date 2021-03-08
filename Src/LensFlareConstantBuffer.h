#pragma once
#include "PixelConstantBuffer.h"
#include "Bindable.h"
#include "DeviceResources.h"

class LensFlareConstantBuffer : public Bindable
{
public:
    __declspec(align(16))
    struct FlareBufferType {
        float brightness;
    };
public:
    LensFlareConstantBuffer(DX::DeviceResources & deviceResources);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
    void Bind() noexcept;
    void Set(FlareBufferType value) { m_value = value; }
private:
    std::unique_ptr<PixelConstantBuffer<FlareBufferType>> m_flareBuffer;
    FlareBufferType m_value;
    DX::DeviceResources & m_deviceResources;
};


#pragma once
#include "Bindable.h"
#include "PixelConstantBuffer.h"

class BloomConstantBuffer : public Bindable
{
public:
    //buffer for bloom parameters
    struct BloomBufferType
    {
        float bloomThreshold;
        float blurAmount;
        float bloomIntensity;
        float baseIntensity;
        float bloomSaturation;
        float baseSaturation;
        uint8_t na[8];
    };

	static constexpr BloomBufferType bloomSetting = { 0.25f, 2.5, 1.1f, 1, 0.6, 1 };

public:
	BloomConstantBuffer(DX::DeviceResources& deviceResources);
	void Bind(DX::DeviceResources& deviceResources) noexcept override;
    void Set();
private:
	std::unique_ptr<PixelConstantBuffer<BloomBufferType>> m_bloomBuffer;
    DX::DeviceResources & m_deviceResources;
};


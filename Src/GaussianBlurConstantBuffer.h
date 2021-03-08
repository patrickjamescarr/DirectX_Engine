#pragma once
#include "Bindable.h"
#include "PixelConstantBuffer.h"
#include "DeviceResources.h"
#include "BloomConstantBuffer.h"

class GaussianBlurConstantBuffer :
    public Bindable
{
public:
    struct BlurBufferType
    {
        static const size_t SAMPLE_COUNT = 15;

        DirectX::XMFLOAT4 sampleOffsets[SAMPLE_COUNT];
        DirectX::XMFLOAT4 sampleWeights[SAMPLE_COUNT];

        void SetBlurEffectParameters(float dx, float dy,
            const BloomConstantBuffer::BloomBufferType& params)
        {
            sampleWeights[0].x = ComputeGaussian(0, params.blurAmount);
            sampleOffsets[0].x = sampleOffsets[0].y = 0.f;

            float totalWeights = sampleWeights[0].x;

            // Add pairs of additional sample taps, positioned
            // along a line in both directions from the center.
            for (size_t i = 0; i < SAMPLE_COUNT / 2; i++)
            {
                // Store weights for the positive and negative taps.
                float weight = ComputeGaussian(float(i + 1.f), params.blurAmount);

                sampleWeights[i * 2 + 1].x = weight;
                sampleWeights[i * 2 + 2].x = weight;

                totalWeights += weight * 2;

                // To get the maximum amount of blurring from a limited number of
                // pixel shader samples, we take advantage of the bilinear filtering
                // hardware inside the texture fetch unit. If we position our texture
                // coordinates exactly halfway between two texels, the filtering unit
                // will average them for us, giving two samples for the price of one.
                // This allows us to step in units of two texels per sample, rather
                // than just one at a time. The 1.5 offset kicks things off by
                // positioning us nicely in between two texels.
                float sampleOffset = float(i) * 2.f + 1.5f;

                DirectX::SimpleMath::Vector2 delta = DirectX::SimpleMath::Vector2(dx, dy) * sampleOffset;

                // Store texture coordinate offsets for the positive and negative taps.
                sampleOffsets[i * 2 + 1].x = delta.x;
                sampleOffsets[i * 2 + 1].y = delta.y;
                sampleOffsets[i * 2 + 2].x = -delta.x;
                sampleOffsets[i * 2 + 2].y = -delta.y;
            }

            for (size_t i = 0; i < SAMPLE_COUNT; i++)
            {
                sampleWeights[i].x /= totalWeights;
            }
        }

    private:
        float ComputeGaussian(float n, float theta)
        {
            return (float)((1.0 / sqrtf(2 * DirectX::XM_PI * theta))
                * expf(-(n * n) / (2 * theta * theta)));
        }
    };
public:
    GaussianBlurConstantBuffer(DX::DeviceResources& deviceResources, BlurBufferType blurParams);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
    void Set();
private:
    std::unique_ptr<PixelConstantBuffer<BlurBufferType>> m_blurBuffer;
    BlurBufferType m_blurParams;
    DX::DeviceResources & m_deviceResources;
};


#include "pch.h"
#include "LensFlareEffect.h"
#include "ModelLoader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "InputLayout.h"
#include "VertexConstantBuffer.h"
#include "PixelConstantBuffer.h"
#include "RasterizerState.h"
#include "Sampler.h";
#include "Texture.h"

using namespace DirectX::SimpleMath;

LensFlareEffect::LensFlareEffect(DX::DeviceResources & deviceResources, Camera * activeCamera, Light * lightSource)
    : m_activeCamera(activeCamera), m_lightSource(lightSource)
{
    // sprite batch stuff
    m_states = std::make_unique<CommonStates>(deviceResources.GetD3DDevice());
    m_flareSprites = std::make_unique<SpriteBatch>(deviceResources.GetD3DDeviceContext());

    // pixel shader
    m_flareConstantBuffer = std::make_unique<LensFlareConstantBuffer>(deviceResources);
    m_flarePixelShader = std::make_unique<PixelShader>(deviceResources, L"flare_ps.cso");

    // create the flare textures
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex6.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex4.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex2.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex7.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex3.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex5.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex7.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex3.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex5.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex4.dds"));
    m_flareTextures.push_back(std::make_unique<Texture>(deviceResources, L"LensFlare//tex8.dds"));
}

void LensFlareEffect::Draw(DX::DeviceResources & deviceResources) const
{
    auto bufferWidth = deviceResources.GetBackBufferWidth();
    auto bufferHeight = deviceResources.GetBackBufferHeight();

    const auto centerScreen = Vector2(bufferWidth / 2, bufferHeight / 2);

    // convert light source position to screen coordintates
    auto lightScreenPosition = getLightScreenPosition(deviceResources.GetView(), deviceResources.GetProjection(), bufferWidth, bufferHeight);

    if (!lightScreenPosition)
    {
        return;
    }

    // store the light position as a 2d vector.
    auto lightPosition = Vector2(lightScreenPosition->x, lightScreenPosition->y);

    // calculate line from the sun that travels through the center of the screen
    auto sunToCenter = centerScreen - lightPosition;

    // calc brightness from distance
    float brightness = 1 - (Vector2(sunToCenter.x / bufferWidth, sunToCenter.y / bufferHeight).Length() / 0.9f);

    // calculate flare positons
    if (brightness > 0)
    {
        auto flareBuffer = LensFlareConstantBuffer::FlareBufferType();
        flareBuffer.brightness = brightness;

        m_flareConstantBuffer->Set(flareBuffer);

        m_flareSprites->Begin(
            SpriteSortMode_Deferred,
            m_states->NonPremultiplied(),
            nullptr, nullptr, nullptr,
            [=]() {
            m_flarePixelShader->Set();
            m_flareConstantBuffer->Bind();
        }
        );

        // render the flares
        int i = 0;
        for (auto &flare : m_flareTextures)
        {
            auto direction = sunToCenter;
            direction = DirectX::XMVectorScale(direction, i * m_spacing);
            auto flarePosition = lightPosition + direction;
            m_flareSprites->Draw(flare->Get(), flarePosition, nullptr, Colors::Transparent, 0.f, flare->GetOrigin());
            i++;
        }

        m_flareSprites->End();
    }
}

std::unique_ptr <DirectX::SimpleMath::Vector3> LensFlareEffect::getLightScreenPosition(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection, UINT bufferWidth, UINT bufferHeight) const
{
    // convert light source position to screen coordintates
    auto lightPosition = Vector4(m_lightSource->getPosition().x, m_lightSource->getPosition().y, m_lightSource->getPosition().z, 1.0f);

    lightPosition = Vector4::Transform(lightPosition, view);
    lightPosition = Vector4::Transform(lightPosition, projection);

    // point off the screen. return null
    if (lightPosition.w <= 0)
    {
        return nullptr;
    }

    lightPosition.Normalize();

    // convert the normalized coords back to screen space
    lightPosition.x = (lightPosition.x / lightPosition.w + 1) / 2.0f;
    lightPosition.y = 1 - ((lightPosition.y / lightPosition.w + 1) / 2.0f);

    auto lightScreenPosition = Vector3(lightPosition.x, lightPosition.y, 1.0f);

    // scale to the backbuffer dimentions
    lightScreenPosition = Vector3::Transform(lightScreenPosition, Matrix::CreateScale(bufferWidth, bufferHeight, 1.0f));

    return std::make_unique<DirectX::SimpleMath::Vector3>(lightScreenPosition);
}



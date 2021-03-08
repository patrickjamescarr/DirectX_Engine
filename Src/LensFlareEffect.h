#pragma once
#include "GameObjectBase.h"
#include "DeviceResources.h"
#include "SceneNode.h"
#include "ModelLoader.h"
#include "Camera.h"
#include "Light.h"
#include "Texture.h"
#include "LensFlareConstantBuffer.h"
#include "PixelShader.h"

class LensFlareEffect
{
public:
    LensFlareEffect(DX::DeviceResources & deviceResources, Camera * activeCamera, Light * lightSource);
    void Draw(DX::DeviceResources & deviceResources) const;
private:
    std::unique_ptr < DirectX::SimpleMath::Vector3> getLightScreenPosition(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix projection, UINT bufferWidth, UINT bufferHeight) const;

    std::unique_ptr<DirectX::SpriteBatch> m_flareSprites;
    std::vector<std::unique_ptr<Texture>> m_flareTextures;

    std::unique_ptr<DirectX::CommonStates> m_states;

    std::unique_ptr<LensFlareConstantBuffer> m_flareConstantBuffer;
    std::unique_ptr<PixelShader> m_flarePixelShader;
    
    Camera* m_activeCamera;
    Light* m_lightSource;
    float m_spacing = 0.6f;
};


#pragma once
#include "GameObject.h"
#include "IndexBuffer.h"
#include "RenderTarget.h"

class OnScreenQuad :
    public GameObject
{
public:
    OnScreenQuad(
        DX::DeviceResources &deviceResources, 
        float width, 
        float height, 
        DirectX::SimpleMath::Matrix transform, 
        RenderTarget* renderTarget
    );
    void Draw(DX::DeviceResources & deviceResources);
    DirectX::SimpleMath::Matrix GetTransform() const noexcept override;
private:
    int m_width;
    int m_height;
    std::vector<std::unique_ptr<Bindable>> m_binds;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    mutable DirectX::SimpleMath::Matrix m_transform;
    std::unique_ptr<DirectX::CommonStates> m_states;
    RenderTarget* m_renderTarget;
};


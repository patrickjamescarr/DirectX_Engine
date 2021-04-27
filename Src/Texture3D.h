#pragma once
#include "Bindable.h"
class Texture3D :
    public Bindable
{
public:
    Texture3D(DX::DeviceResources & deviceResource, const wchar_t* fileName, UINT slot);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
    ID3D11ShaderResourceView* Get() const { return m_textureSrv.Get(); }
    DirectX::SimpleMath::Vector2 GetOrigin() const;
private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureSrv;
    Microsoft::WRL::ComPtr<ID3D11Texture3D> m_texture;
    DirectX::SimpleMath::Vector2 m_origin;
    DirectX::SimpleMath::Vector2 m_position;
    UINT m_slot;

};


#pragma once
#include "Bindable.h"

class StreamOutput
    : public Bindable
{
public:
    StreamOutput(DX::DeviceResources& deviceResources, const int& bufferSize);
    void Bind(DX::DeviceResources& deviceResources) noexcept override;
    ID3D11Buffer* GetOutputBuffer() const { return m_streamOutputBuffer.Get(); }
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_streamOutputBuffer;
};


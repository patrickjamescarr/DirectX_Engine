#include "pch.h"
#include "StreamOutput.h"

StreamOutput::StreamOutput(DX::DeviceResources& deviceResources, const int& bufferSize)
{
    int m_nBufferSize = bufferSize;

    D3D11_BUFFER_DESC bufferDesc =
    {
        m_nBufferSize,
        D3D11_USAGE_DEFAULT,
        D3D11_BIND_STREAM_OUTPUT,
        0,
        0,
        0
    };
    deviceResources.GetD3DDevice()->CreateBuffer(&bufferDesc, NULL, &m_streamOutputBuffer);
}

void StreamOutput::Bind(DX::DeviceResources & deviceResources) noexcept
{
    UINT offset[1] = { 0 };
    deviceResources.GetD3DDeviceContext()->SOSetTargets(1, m_streamOutputBuffer.GetAddressOf(), offset);
}

#include "pch.h"
#include "DepthStencilState.h"

DepthStencilState::DepthStencilState(DX::DeviceResources & deviceResources, State state)
{
    auto depthStencilDesc = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };

    if (state == DepthStencilState::depthOff)
    {
        depthStencilDesc.DepthEnable = false;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    }

    DX::ThrowIfFailed(
        deviceResources.GetD3DDevice()->CreateDepthStencilState(
            &depthStencilDesc,
            m_state.GetAddressOf()
        )
    );
}

void DepthStencilState::Bind(DX::DeviceResources & deviceResources) noexcept
{
    deviceResources.GetD3DDeviceContext()->OMSetDepthStencilState(m_state.Get(), 0xFF);
}

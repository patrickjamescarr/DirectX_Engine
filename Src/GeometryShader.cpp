#include "pch.h"
#include "GeometryShader.h"

GeometryShader::GeometryShader(DX::DeviceResources& deviceResources, const WCHAR * fileName, bool streamOutput)
{
    auto bytecode = std::make_unique<std::vector<uint8_t>>(DX::ReadData(fileName));

    if (streamOutput) {

        D3D11_SO_DECLARATION_ENTRY pDecl[] =
        {
            // semantic name, semantic index, start component, component count, output slot
            // matches GS output
            { 0, "SV_POSITION", 0, 0, 4, 0 },
            { 0, "CAMPOSITION", 0, 0, 4, 0 },
            { 0, "DENSITY", 0, 0, 4, 0 },
            { 0, "DENSITY", 1, 0, 4, 0 }
        };

        DX::ThrowIfFailed(
            deviceResources.GetD3DDevice()->CreateGeometryShaderWithStreamOutput(
                bytecode->data(),
                bytecode->size(),
                pDecl, _countof(pDecl), NULL, 0, 0, NULL, &m_geometryShader
            )
        );
    } 
    else {
        DX::ThrowIfFailed(
            deviceResources.GetD3DDevice()->CreateGeometryShader(
                bytecode->data(),
                bytecode->size(),
                NULL, &m_geometryShader
            )
        );
    }
}

void GeometryShader::Bind(DX::DeviceResources & deviceResources) noexcept
{
    deviceResources.GetD3DDeviceContext()->GSSetShader(m_geometryShader.Get(), 0, 0);
}
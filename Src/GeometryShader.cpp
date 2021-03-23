#include "pch.h"
#include "GeometryShader.h"

GeometryShader::GeometryShader(DX::DeviceResources& deviceResources, const WCHAR * fileName)
{
    auto bytecode = std::make_unique<std::vector<uint8_t>>(DX::ReadData(fileName));

    DX::ThrowIfFailed(
        deviceResources.GetD3DDevice()->CreateGeometryShader(
            bytecode->data(),
            bytecode->size(),
            NULL, &m_geometryShader
        )
    );
}

void GeometryShader::Bind(DX::DeviceResources & deviceResources) noexcept
{
    deviceResources.GetD3DDeviceContext()->GSSetShader(m_geometryShader.Get(), 0, 0);
}
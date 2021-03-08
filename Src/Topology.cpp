#include "pch.h"
#include "Topology.h"

Topology::Topology(DX::DeviceResources & deviceResources, D3D11_PRIMITIVE_TOPOLOGY topology) : topology(topology)
{
}

void Topology::Bind(DX::DeviceResources & deviceResources) noexcept
{
	deviceResources.GetD3DDeviceContext()->IASetPrimitiveTopology(topology);
}

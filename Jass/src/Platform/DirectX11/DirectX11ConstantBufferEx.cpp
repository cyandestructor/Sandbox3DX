#include "jasspch.h"
#include "DirectX11ConstantBufferEx.h"

namespace Jass {

	DirectX11ConstantBufferEx::DirectX11ConstantBufferEx(Dcb::RawLayout&& layout, Destination destination, unsigned int slot)
		:m_buffer(std::move(layout)), m_destination(destination), m_slot(slot)
	{
		D3D11_BUFFER_DESC bDesc = {};
		bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bDesc.Usage = D3D11_USAGE_DYNAMIC;	
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bDesc.MiscFlags = 0u;
		bDesc.ByteWidth = (unsigned int)m_buffer.GetSizeInBytes();

		auto device = DirectX11Graphics::Get().GetDevice();
		device->CreateBuffer(&bDesc, nullptr, &m_constantBuffer);
	}

	void DirectX11ConstantBufferEx::Bind() const
	{
		auto deviceContext = DirectX11Graphics::Get().GetDeviceContext();

		Update(deviceContext);

		switch (m_destination)
		{
		case Destination::VertexShader:
			deviceContext->VSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
			break;
		case Destination::PixelShader:
			deviceContext->PSSetConstantBuffers(m_slot, 1u, m_constantBuffer.GetAddressOf());
			break;
		}
	}

	void DirectX11ConstantBufferEx::Update(ComPtr<ID3D11DeviceContext> deviceContext) const
	{
		D3D11_MAPPED_SUBRESOURCE msr = {};
		deviceContext->Map(m_constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
		size_t size = m_buffer.GetSizeInBytes();
		memcpy_s(msr.pData, size, m_buffer.GetData(), size);
		deviceContext->Unmap(m_constantBuffer.Get(), 0u);
	}

}

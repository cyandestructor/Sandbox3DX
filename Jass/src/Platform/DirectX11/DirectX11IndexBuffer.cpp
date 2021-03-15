#include "jasspch.h"
#include "DirectX11IndexBuffer.h"

namespace Jass {

	static D3D11_USAGE DX11DataUsage(DataUsage usage)
	{
		// Temporary
		return D3D11_USAGE_DEFAULT;
	}

	DirectX11IndexBuffer::DirectX11IndexBuffer(const IndexBufferConfig& config)
		:m_count(config.Count)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();

		D3D11_BUFFER_DESC bDesc = {};
		bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bDesc.Usage = DX11DataUsage(config.DataUsage);
		bDesc.CPUAccessFlags = 0u;
		bDesc.MiscFlags = 0u;
		bDesc.ByteWidth = config.Count * (unsigned int)sizeof(unsigned int);

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = config.Data;

		device->CreateBuffer(&bDesc, &sd, &m_indexBuffer);
	}

	void DirectX11IndexBuffer::Bind() const
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();

		deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}

	void DirectX11IndexBuffer::Unbind() const
	{
	}

}
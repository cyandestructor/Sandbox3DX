#include "jasspch.h"
#include "DirectX11VertexBuffer.h"

namespace Jass {

	static D3D11_USAGE DX11DataUsage(DataUsage usage)
	{
		// Temporary
		return D3D11_USAGE_DEFAULT;
	}

	DirectX11VertexBuffer::DirectX11VertexBuffer(const VertexBufferConfig& config)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();

		D3D11_BUFFER_DESC bDesc = {};
		bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bDesc.Usage = DX11DataUsage(config.DataUsage);
		bDesc.CPUAccessFlags = 0u;
		bDesc.MiscFlags = 0u;
		bDesc.ByteWidth = config.Size;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = config.Data;

		device->CreateBuffer(&bDesc, &sd, &m_vertexBuffer);
	}

	DirectX11VertexBuffer::DirectX11VertexBuffer(unsigned int size)
	{
		JASS_CORE_ASSERT(false, "Not yet implemented");
	}

	void DirectX11VertexBuffer::Bind() const
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();
		
		const unsigned int stride = m_layout.GetStride();
		const unsigned int offset = 0u;

		deviceContext->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	}

	void DirectX11VertexBuffer::Unbind() const
	{
	}

	void DirectX11VertexBuffer::SetData(const void* data, unsigned int size)
	{
		JASS_CORE_ASSERT(false, "Not yet implemented");
	}

}

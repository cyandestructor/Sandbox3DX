#ifndef DIRECTX11_VERTEX_BUFFER_H
#define DIRECTX11_VERTEX_BUFFER_H

#include "Jass/Renderer/VertexBuffer.h"

#include "DirectX11Graphics.h"

namespace Jass {

	class JASS_API DirectX11VertexBuffer : public VertexBuffer
	{
	public:
		DirectX11VertexBuffer(const VertexBufferConfig& config);
		DirectX11VertexBuffer(unsigned int size);
		virtual ~DirectX11VertexBuffer() = default;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetLayout(const BufferLayout& layout) override { m_layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return m_layout; }

		virtual void SetData(const void* data, unsigned int size) override;

	private:
		ComPtr<ID3D11Buffer> m_vertexBuffer;
		BufferLayout m_layout;

	};

}

#endif // !DIRECTX11_VERTEX_BUFFER_H

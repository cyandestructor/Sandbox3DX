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

		virtual void SetLayout(const BufferLayout& layout) override;
		virtual const BufferLayout& GetLayout() const override { return m_layout; }

		virtual void SetData(const void* data, unsigned int size) override;

		ComPtr<ID3D11Buffer> GetDirectX11Buffer() const { return m_vertexBuffer; }

	private:
		ComPtr<ID3D11Buffer> m_vertexBuffer;
		ComPtr<ID3D11InputLayout> m_inputLayout;
		BufferLayout m_layout;

		void AddLayoutElement(const BufferElement& layoutElement, std::vector<D3D11_INPUT_ELEMENT_DESC>& elements);
		ComPtr<ID3DBlob> CreateDummyShader(const BufferLayout& layout);

	};

}

#endif // !DIRECTX11_VERTEX_BUFFER_H

#ifndef DIRECTX11_VERTEX_ARRAY_H
#define DIRECTX11_VERTEX_ARRAY_H

#include "Jass/Renderer/VertexArray.h"

namespace Jass {

	class JASS_API DirectX11VertexArray : public VertexArray {

	public:
		DirectX11VertexArray() = default;
		virtual ~DirectX11VertexArray() = default;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override
		{
			return m_vertexBuffers;
		}

		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override
		{
			return m_indexBuffer;
		}

	private:
		std::vector<Ref<VertexBuffer>> m_vertexBuffers;
		Ref<IndexBuffer> m_indexBuffer;

	};

}

#endif // !DIRECTX11_VERTEX_ARRAY_H

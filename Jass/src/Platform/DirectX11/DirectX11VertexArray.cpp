#include "jasspch.h"
#include "DirectX11VertexArray.h"

namespace Jass {

	void DirectX11VertexArray::Bind() const
	{
	}

	void DirectX11VertexArray::Unbind() const
	{
	}

	void DirectX11VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		JASS_CORE_ASSERT(vertexBuffer != nullptr, "Vertex buffer should not be nullptr");
		JASS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer has no layout");

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void DirectX11VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		JASS_CORE_ASSERT(indexBuffer != nullptr, "Index buffer should not be nullptr");
		m_indexBuffer = indexBuffer;
	}

}

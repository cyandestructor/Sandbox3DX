#include "jasspch.h"
#include "IndexBuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/DirectX11/DirectX11IndexBuffer.h"

namespace Jass {

	Ref<IndexBuffer> IndexBuffer::Create(const IndexBufferConfig& config)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				JASS_CORE_ASSERT(false, "Renderer API None is currently not supported");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return MakeRef<OpenGLIndexBuffer>(config);
			case RendererAPI::API::DirectX11:
				return MakeRef<DirectX11IndexBuffer>(config);
		}

		JASS_CORE_ASSERT(false, "Unknow Renderer API");
		return nullptr;
	}

}


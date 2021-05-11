#include "jasspch.h"
#include "Framebuffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/DirectX11/DirectX11Framebuffer.h"

namespace Jass {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferConfig& config)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				JASS_CORE_ASSERT(false, "Renderer API None is currently not supported");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return MakeRef<OpenGLFramebuffer>(config);
			case RendererAPI::API::DirectX11:
				return MakeRef<DirectX11Framebuffer>(config);
		}

		JASS_CORE_ASSERT(false, "Unknow Renderer API");
		return nullptr;
	}

}
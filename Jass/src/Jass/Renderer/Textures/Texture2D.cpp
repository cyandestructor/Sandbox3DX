#include "jasspch.h"
#include "Texture2D.h"

#include "Jass/Renderer/Renderer.h"
#include "Platform/OpenGL/Textures/OpenGLTexture2D.h"
#include "Platform/DirectX11/Textures/DirectX11Texture2D.h"

namespace Jass {

	Ref<Texture2D> Texture2D::Create(unsigned int width, unsigned int height)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				JASS_CORE_ASSERT(false, "Renderer API None is currently not supported");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return MakeRef<OpenGLTexture2D>(width, height);
			case RendererAPI::API::DirectX11:
				return MakeRef<DirectX11Texture2D>(width, height);
		}

		JASS_CORE_ASSERT(false, "Unknow Renderer API");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				JASS_CORE_ASSERT(false, "Renderer API None is currently not supported");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return MakeRef<OpenGLTexture2D>(filepath);
			case RendererAPI::API::DirectX11:
				return MakeRef<DirectX11Texture2D>(filepath);
		}

		JASS_CORE_ASSERT(false, "Unknow Renderer API");
		return nullptr;
	}

}

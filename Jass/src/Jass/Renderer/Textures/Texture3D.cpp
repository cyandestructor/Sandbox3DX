#include "jasspch.h"
#include "Texture3D.h"

#include "Jass/Renderer/Renderer.h"
#include "Platform/OpenGL/Textures/OpenGLTexture3D.h"
#include "Platform/DirectX11/Textures/DirectX11Texture3D.h"

namespace Jass {

	Ref<Texture3D> Texture3D::Create(const std::vector<std::string>& filenames)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			JASS_CORE_ASSERT(false, "Renderer API None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return MakeRef<OpenGLTexture3D>(filenames);
		case RendererAPI::API::DirectX11:
			return MakeRef<DirectX11Texture3D>(filenames);
		}

		JASS_CORE_ASSERT(false, "Unknow Renderer API");
		return nullptr;
	}

}
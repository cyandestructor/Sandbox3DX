#include "jasspch.h"
#include "RenderCommand.h"

//#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/DirectX11/DirectX11RendererAPI.h"

namespace Jass {

	// TODO: Make it dynamic for other API's

	std::unique_ptr<RendererAPI> RenderCommand::s_rendererAPI = std::make_unique<DirectX11RendererAPI>();

}
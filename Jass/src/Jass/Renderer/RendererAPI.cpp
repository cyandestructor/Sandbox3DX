#include "jasspch.h"
#include "RendererAPI.h"

namespace Jass {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::DirectX11;

	RendererAPI::API RendererAPI::GetAPI()
	{
		return s_API;
	}

}
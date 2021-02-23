#include "jasspch.h"
#include "DirectX11Context.h"

#include "DirectX11Graphics.h"

namespace Jass {

	DirectX11Context::DirectX11Context(HWND windowHandler)
		:m_windowHandler(windowHandler)
	{
		JASS_ASSERT(windowHandler, "The window handler is null");
	}

	void DirectX11Context::Init()
	{
		DirectX11Graphics::Init(m_windowHandler);
	}

	void DirectX11Context::SwapBuffers()
	{
		DirectX11Graphics::Get().GetSwapChain()->Present(1u, 0u);
	}

}


#include "jasspch.h"
#include "DirectX11Context.h"

#include "DirectX11Graphics.h"

#include "DynamicConstantBuffer/DCBTesting.h"

namespace Jass {

	DirectX11Context::DirectX11Context(HWND windowHandler)
		:m_windowHandler(windowHandler)
	{
		JASS_ASSERT(windowHandler, "The window handler is null");
		TestDCB();
	}

	void DirectX11Context::Init()
	{
		DirectX11Graphics::Init(m_windowHandler);
	}

	void DirectX11Context::SwapBuffers()
	{
		auto& graphics = DirectX11Graphics::Get();
		if (graphics.IsVSync())
			graphics.GetSwapChain()->Present(1u, 0u);
		else
			graphics.GetSwapChain()->Present(0u, 0u);
	}

	void DirectX11Context::SetVSync(bool enable)
	{
		DirectX11Graphics::Get().SetVSync(enable);
	}

	bool DirectX11Context::IsVSync() const
	{
		return DirectX11Graphics::Get().IsVSync();
	}

}


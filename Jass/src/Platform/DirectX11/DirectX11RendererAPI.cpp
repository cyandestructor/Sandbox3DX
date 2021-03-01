#include "jasspch.h"
#include "DirectX11RendererAPI.h"

#include "Platform/DirectX11/DirectX11Graphics.h"

namespace Jass {

	void DirectX11RendererAPI::Init()
	{
	}

	void DirectX11RendererAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
	}

	void DirectX11RendererAPI::SetClearColor(const JVec4& color)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto rtv = graphics.GetRenderTargetView();
		graphics.GetDeviceContext()->ClearRenderTargetView(rtv.Get(), GetPtr(color));
	}

	void DirectX11RendererAPI::Clear()
	{
	}

	void DirectX11RendererAPI::EnableClipDistance(bool enable, unsigned int index)
	{
	}

	void DirectX11RendererAPI::EnableDepthMask(bool enable)
	{
	}

	void DirectX11RendererAPI::SetDepthFunction(DepthFunc function)
	{
	}

	void DirectX11RendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount, RenderMode renderMode)
	{
	}

}
#include "jasspch.h"
#include "DirectX11RendererAPI.h"

#include "Platform/DirectX11/DirectX11Graphics.h"

namespace Jass {

	static D3D11_PRIMITIVE_TOPOLOGY ToDirectXTopology(RenderMode renderMode)
	{
		D3D11_PRIMITIVE_TOPOLOGY mode = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		switch (renderMode)
		{
		case RenderMode::Triangle:
			mode = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case RenderMode::TriangleStrip:
			mode = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		default:
			JASS_ASSERT(false, "Unknown render mode");
			break;
		}

		return mode;
	}

	static D3D11_COMPARISON_FUNC ToDirectXComparisonFunc(DepthFunc func)
	{
		D3D11_COMPARISON_FUNC mode = D3D11_COMPARISON_NEVER;

		switch (func)
		{
		case DepthFunc::Never:
			mode = D3D11_COMPARISON_NEVER;
			break;
		case DepthFunc::Less:
			mode = D3D11_COMPARISON_LESS;
			break;
		case DepthFunc::Equal:
			mode = D3D11_COMPARISON_EQUAL;
			break;
		case DepthFunc::LessEqual:
			mode = D3D11_COMPARISON_LESS_EQUAL;
			break;
		case DepthFunc::NotEqual:
			mode = D3D11_COMPARISON_NOT_EQUAL;
			break;
		case DepthFunc::Greater:
			mode = D3D11_COMPARISON_GREATER;
			break;
		case DepthFunc::GreaterEqual:
			mode = D3D11_COMPARISON_GREATER_EQUAL;
			break;
		case DepthFunc::Always:
			mode = D3D11_COMPARISON_ALWAYS;
			break;
		default:
			JASS_CORE_ASSERT(false, "Unknown depth function");
			break;
		}

		return mode;
	}

	static D3D11_CULL_MODE ToDirectXCullMode(CullMode cullMode)
	{
		switch (cullMode)
		{
		case CullMode::None:
			return D3D11_CULL_NONE;
		case CullMode::Front:
			return D3D11_CULL_FRONT;
		case CullMode::Back:
			return D3D11_CULL_BACK;
		default:
			JASS_CORE_ASSERT(false, "Unknown depth function");
		}

		return D3D11_CULL_NONE;
	}

	void DirectX11RendererAPI::Init()
	{
	}

	void DirectX11RendererAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		auto& graphics = DirectX11Graphics::Get();

		auto viewport = graphics.GetViewport();
		viewport.TopLeftX = (float)x;
		viewport.TopLeftY = (float)y;
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		
		graphics.SetViewport(viewport);
	}

	void DirectX11RendererAPI::SetClearColor(const JVec4& color)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();

		ComPtr<ID3D11RenderTargetView> rtv;
		deviceContext->OMGetRenderTargets(1u, &rtv, nullptr);

		deviceContext->ClearRenderTargetView(rtv.Get(), GetPtr(color));
	}

	void DirectX11RendererAPI::Clear()
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();
		
		ComPtr<ID3D11DepthStencilView> dsv;
		deviceContext->OMGetRenderTargets(1u, nullptr, &dsv);

		deviceContext->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	}

	void DirectX11RendererAPI::SetCullMode(CullMode mode)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();
		auto deviceContext = graphics.GetDeviceContext();

		ComPtr<ID3D11RasterizerState> rs;
		deviceContext->RSGetState(&rs);

		D3D11_RASTERIZER_DESC rd = {};
		rs->GetDesc(&rd);

		rd.CullMode = ToDirectXCullMode(mode);

		ComPtr<ID3D11RasterizerState> newRs;
		device->CreateRasterizerState(&rd, &newRs);

		deviceContext->RSSetState(newRs.Get());
	}

	void DirectX11RendererAPI::EnableClipDistance(bool enable, unsigned int index)
	{
	}

	void DirectX11RendererAPI::EnableDepthMask(bool enable)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();
		auto deviceContext = graphics.GetDeviceContext();

		// Get the current depth stencil state
		ComPtr<ID3D11DepthStencilState> dsState;
		unsigned int stencilRef = 1u;
		deviceContext->OMGetDepthStencilState(&dsState, &stencilRef);

		// Get the currect depth stencil description
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsState->GetDesc(&dsDesc);
		// Enable/Disable depth mask
		dsDesc.DepthWriteMask = enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;

		// Create a new depth stencil state with the new function
		device->CreateDepthStencilState(&dsDesc, &dsState);

		// Bind the new depth stencil state
		deviceContext->OMSetDepthStencilState(dsState.Get(), 1u);
	}

	void DirectX11RendererAPI::SetDepthFunction(DepthFunc function)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();
		auto deviceContext = graphics.GetDeviceContext();
		
		// Get the current depth stencil state
		ComPtr<ID3D11DepthStencilState> dsState;
		unsigned int stencilRef = 1u;
		deviceContext->OMGetDepthStencilState(&dsState, &stencilRef);

		// Get the currect depth stencil description
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsState->GetDesc(&dsDesc);
		// Change the depth function
		dsDesc.DepthFunc = ToDirectXComparisonFunc(function);

		// Create a new depth stencil state with the new function
		device->CreateDepthStencilState(&dsDesc, &dsState);

		// Bind the new depth stencil state
		deviceContext->OMSetDepthStencilState(dsState.Get(), 1u);
	}

	void DirectX11RendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount, RenderMode renderMode)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();

		deviceContext->IASetPrimitiveTopology(ToDirectXTopology(renderMode));

		unsigned int count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();

		vertexArray->GetIndexBuffer()->Bind();

		for (const auto& vertexBuffer : vertexArray->GetVertexBuffers()) {
			vertexBuffer->Bind();
			deviceContext->DrawIndexed(count, 0u, 0);
		}
	}

}
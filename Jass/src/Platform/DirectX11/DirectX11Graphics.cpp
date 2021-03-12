#include "jasspch.h"
#include "DirectX11Graphics.h"

#pragma comment(lib, "d3d11.lib")

namespace Jass {

	Scope<DirectX11Graphics> DirectX11Graphics::s_graphics = nullptr;

	DirectX11Graphics::DirectX11Graphics(HWND hWnd)
	{
		CreateDeviceAndSwapChain(hWnd);
		CreateRenderTargetView();
		CreateDepthStencilView();

		m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	}

	void DirectX11Graphics::Init(HWND hWnd)
	{
		s_graphics = MakeScope<DirectX11Graphics>(hWnd);
	}

	DirectX11Graphics& DirectX11Graphics::Get()
	{
		JASS_ASSERT(s_graphics, "Graphics must be initialized");

		return *s_graphics;
	}

	void DirectX11Graphics::CreateDeviceAndSwapChain(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC scDesc = {};
		scDesc.BufferDesc.Width = 0;
		scDesc.BufferDesc.Height = 0;
		scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scDesc.BufferDesc.RefreshRate.Numerator = 0;
		scDesc.BufferDesc.RefreshRate.Denominator = 0;
		scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.BufferCount = 1;
		scDesc.OutputWindow = hWnd;
		scDesc.Windowed = TRUE;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scDesc.Flags = 0;

		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&scDesc,
			&m_swapChain,
			&m_device,
			nullptr,
			&m_deviceContext
		);
	}

	void DirectX11Graphics::CreateRenderTargetView()
	{
		ComPtr<ID3D11Resource> backBuffer = nullptr;
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);
		m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
	}

	void DirectX11Graphics::CreateDepthStencilView()
	{
		// Create and Set the Depth Stencil State
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		ComPtr<ID3D11DepthStencilState> dsState;
		m_device->CreateDepthStencilState(&dsDesc, &dsState);

		m_deviceContext->OMSetDepthStencilState(dsState.Get(), 1u);

		// Get the Swap Chain Description to get the width and height
		DXGI_SWAP_CHAIN_DESC scDesc = {};
		m_swapChain->GetDesc(&scDesc);

		unsigned int width = scDesc.BufferDesc.Width, height = scDesc.BufferDesc.Height;

		// Create the Depth Texture
		D3D11_TEXTURE2D_DESC dtDesc = {};
		dtDesc.Width = width;
		dtDesc.Height = height;
		dtDesc.MipLevels = 1u;
		dtDesc.ArraySize = 1u;
		dtDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ComPtr<ID3D11Texture2D> dsTexture;
		m_device->CreateTexture2D(&dtDesc, nullptr, &dsTexture);

		// Create the Depth Stencil View
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0u;

		m_device->CreateDepthStencilView(dsTexture.Get(), &dsvDesc, &m_depthStencilView);
	}

}

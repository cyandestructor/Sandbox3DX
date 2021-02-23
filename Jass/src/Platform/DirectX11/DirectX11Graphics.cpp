#include "jasspch.h"
#include "DirectX11Graphics.h"

#pragma comment(lib, "d3d11.lib")

namespace Jass {

	Scope<DirectX11Graphics> DirectX11Graphics::s_graphics = nullptr;

	DirectX11Graphics::DirectX11Graphics(HWND hWnd)
	{
		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 0;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = hWnd;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			&m_swapChain,
			&m_device,
			nullptr,
			&m_deviceContext
		);

		ComPtr<ID3D11Resource> backBuffer = nullptr;
		m_swapChain->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer);
		m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);
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

}

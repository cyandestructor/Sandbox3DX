#ifndef DIRECTX11GRAPHICS_H
#define DIRECTX11GRAPHICS_H

#include <d3d11.h>
#include <wrl.h>

namespace Jass {

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	class DirectX11Graphics {

	public:
		DirectX11Graphics(HWND hWnd);

		static void Init(HWND hWnd);
		static DirectX11Graphics& Get();

		ComPtr<ID3D11Device> GetDevice() { return m_device; }
		ComPtr<IDXGISwapChain> GetSwapChain() { return m_swapChain; }
		ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_deviceContext; }
		ComPtr<ID3D11RenderTargetView> GetRenderTargetView() { return m_renderTargetView; }
		ComPtr<ID3D11DepthStencilView> GetDepthStencilView() { return m_depthStencilView; }

		D3D11_VIEWPORT GetViewport() const { return m_viewport; }
		void SetViewport(D3D11_VIEWPORT viewport);

		bool IsVSync() const { return m_vSync; }
		void SetVSync(bool enable) { m_vSync = enable; }

	private:
		static Scope<DirectX11Graphics> s_graphics;

		bool m_vSync = true;

		D3D11_VIEWPORT m_viewport;

		ComPtr<ID3D11Device> m_device;
		ComPtr<IDXGISwapChain> m_swapChain;
		ComPtr<ID3D11DeviceContext> m_deviceContext;
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		void CreateDeviceAndSwapChain(HWND hWnd);
		void CreateRenderTargetView();
		void CreateDepthStencilView();
		void CreateViewport();
		void CreateRasterizerState();

	};

}

#endif // !DIRECTX11GRAPHICS_H

#include "jasspch.h"
#include "DirectX11Framebuffer.h"

namespace Jass {

	DirectX11Framebuffer::DirectX11Framebuffer(const FramebufferConfig& config)
		:m_fbConfig(config)
	{
		Invalidate();
	}

	DirectX11Framebuffer::~DirectX11Framebuffer()
	{
	}

	void DirectX11Framebuffer::Resize(unsigned int width, unsigned int height)
	{
		m_fbConfig.Width = width;
		m_fbConfig.Height = height;

		Invalidate();
	}

	void DirectX11Framebuffer::BindColorAttachment(unsigned int slot) const
	{
		auto deviceContext = DirectX11Graphics::Get().GetDeviceContext();
		deviceContext->PSSetShaderResources(slot, 1u, m_colorShaderResourceView.GetAddressOf());
		m_slot = slot;
	}

	void DirectX11Framebuffer::Bind() const
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();
		auto dsv = graphics.GetDepthStencilView();

		// If the color attachment has been bound, clear that shader resource slot
		// before writing to it with the rtv
		if (m_slot != -1)
		{
			ComPtr<ID3D11ShaderResourceView> nullSrv = nullptr;
			deviceContext->PSSetShaderResources(m_slot, 1u, nullSrv.GetAddressOf());
		}

		deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), dsv.Get());
	}

	void DirectX11Framebuffer::Unbind() const
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();

		auto rtv = graphics.GetRenderTargetView();
		auto dsv = graphics.GetDepthStencilView();

		// Bind the original views again
		deviceContext->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
	}

	void DirectX11Framebuffer::Invalidate()
	{
		auto rtTexture = InitRenderTargetView();
		auto dsTexture = InitDepthStencilView();
		InitShaderResourceViews(rtTexture, dsTexture);
	}

	ComPtr<ID3D11Texture2D> DirectX11Framebuffer::InitRenderTargetView()
	{
		HRESULT result;

		auto device = DirectX11Graphics::Get().GetDevice();

		ComPtr<ID3D11Texture2D> renderTargetTexture;

		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = m_fbConfig.Width;
		tDesc.Height = m_fbConfig.Height;
		tDesc.MipLevels = 1u;
		tDesc.ArraySize = 1u;
		tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tDesc.SampleDesc.Count = 1u;
		tDesc.SampleDesc.Quality = 0u;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		tDesc.CPUAccessFlags = 0u;
		tDesc.MiscFlags = 0u;
		
		result = device->CreateTexture2D(&tDesc, nullptr, &renderTargetTexture);
		JASS_CORE_ASSERT(SUCCEEDED(result), "Operation failed");

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = tDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0u;

		result = device->CreateRenderTargetView(renderTargetTexture.Get(), &rtvDesc, &m_renderTargetView);
		JASS_CORE_ASSERT(SUCCEEDED(result), "Operation failed");

		return renderTargetTexture;
	}

	ComPtr<ID3D11Texture2D> DirectX11Framebuffer::InitDepthStencilView()
	{
		HRESULT result;

		auto device = DirectX11Graphics::Get().GetDevice();

		ComPtr<ID3D11Texture2D> depthStencilTexture;

		D3D11_TEXTURE2D_DESC dtDesc = {};
		dtDesc.Width = m_fbConfig.Width;
		dtDesc.Height = m_fbConfig.Height;
		dtDesc.MipLevels = 1u;
		dtDesc.ArraySize = 1u;
		dtDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dtDesc.SampleDesc.Count = 1u;
		dtDesc.SampleDesc.Quality = 0u;
		dtDesc.Usage = D3D11_USAGE_DEFAULT;
		dtDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		result = device->CreateTexture2D(&dtDesc, nullptr, &depthStencilTexture);
		JASS_CORE_ASSERT(SUCCEEDED(result), "Operation failed");

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dtDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0u;

		result = device->CreateDepthStencilView(depthStencilTexture.Get(), &dsvDesc, &m_depthStencilView);
		JASS_CORE_ASSERT(SUCCEEDED(result), "Operation failed");

		return depthStencilTexture;
	}

	void DirectX11Framebuffer::InitShaderResourceViews(ComPtr<ID3D11Texture2D> rtTexture, ComPtr<ID3D11Texture2D> dsTexture)
	{
		HRESULT result;

		auto device = DirectX11Graphics::Get().GetDevice();
		
		// Render target
		D3D11_TEXTURE2D_DESC tDesc = {};
		rtTexture->GetDesc(&tDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC colorSrvDesc = {};
		colorSrvDesc.Format = tDesc.Format;
		colorSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		colorSrvDesc.Texture2D.MipLevels = 1u;
		colorSrvDesc.Texture2D.MostDetailedMip = 0u;

		result = device->CreateShaderResourceView(rtTexture.Get(), &colorSrvDesc, &m_colorShaderResourceView);
		JASS_CORE_ASSERT(SUCCEEDED(result), "Operation failed");
	}

}

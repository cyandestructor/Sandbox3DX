#ifndef DIRECTX11_FRAMEBUFFER_H
#define DIRECTX11_FRAMEBUFFER_H

#include "Jass/Renderer/Framebuffer.h"

#include "DirectX11Graphics.h"

namespace Jass {

	class JASS_API DirectX11Framebuffer : public Framebuffer {

	public:
		DirectX11Framebuffer(const FramebufferConfig& config);
		virtual ~DirectX11Framebuffer();

		virtual const FramebufferConfig& GetConfig() const override { return m_fbConfig; }

		virtual void Resize(unsigned int width, unsigned int height) override;

		virtual unsigned int GetColorAttachmentRendererID() const override { return m_colorAttachment; }
		virtual void BindColorAttachment(unsigned int slot = 0) const override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void Invalidate();

	private:
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		ComPtr<ID3D11ShaderResourceView> m_colorShaderResourceView;
		ComPtr<ID3D11ShaderResourceView> m_depthShaderResourceView;
		
		mutable unsigned int m_slot = -1;
		unsigned int m_colorAttachment = 0;
		FramebufferConfig m_fbConfig;

		ComPtr<ID3D11Texture2D> InitRenderTargetView();
		ComPtr<ID3D11Texture2D> InitDepthStencilView();
		void InitShaderResourceViews(ComPtr<ID3D11Texture2D> rtTexture, ComPtr<ID3D11Texture2D>dsTexture);
	};

}

#endif // !DIRECTX11_FRAMEBUFFER_H

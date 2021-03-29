#ifndef DIRECTX11_TEXTURE_2D_H
#define DIRECTX11_TEXTURE_2D_H

#include "Jass/Renderer/Textures/Texture2D.h"

#include "../DirectX11Graphics.h"

namespace Jass {

	class DirectX11Texture2D : public Texture2D {

	public:
		DirectX11Texture2D(unsigned int width, unsigned int height);
		DirectX11Texture2D(const std::string& filepath);
		virtual ~DirectX11Texture2D() = default;

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }

		virtual void Bind(unsigned int slot = 0) const override;

		virtual void SetData(const void* data, unsigned int size) override;

	protected:
		virtual bool IsEqual(const ITexture& other) const override;

	private:
		std::string m_filepath;
		unsigned int m_width = 0;
		unsigned int m_height = 0;

		ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		ComPtr<ID3D11SamplerState> m_samplerState;

		struct TextureInfo {
			const void* Data;
			DXGI_FORMAT Format;
			unsigned int BPP;
		};

		void InitTexture(const TextureInfo& texInfo);
		void InitSamplerState();

	};

}

#endif // !DIRECTX11_TEXTURE_2D_H

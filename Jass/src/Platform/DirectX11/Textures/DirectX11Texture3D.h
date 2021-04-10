#ifndef DIRECTX11_TEXTURE3D_H_
#define DIRECTX11_TEXTURE3D_H_

#include "Jass/Renderer/Textures/Texture3D.h"

#include "../DirectX11Graphics.h"

namespace Jass {

	class DirectX11Texture3D : public Texture3D {

	public:
		DirectX11Texture3D(const std::vector<std::string>& filenames);
		virtual ~DirectX11Texture3D();


		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }

		virtual void Bind(unsigned int slot = 0) const override;

		virtual void SetData(const void* data, unsigned int size) override;

	protected:
		virtual bool IsEqual(const ITexture& other) const override;

	private:
		static ComPtr<ID3D11SamplerState> s_samplerState;
		static bool s_samplerStateBound;

		static void InitSamplerState();
		
		std::vector<std::string> m_filenames;

		unsigned int m_width = 0;
		unsigned int m_height = 0;

		ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

		struct ImageData {
			unsigned char* Data;
			unsigned int Width;
			unsigned int Height;
			unsigned int Channels;
		};

		std::vector<ImageData> m_images;

		ImageData ProcessImage(const std::string& filename);
		void FreeImage(ImageData image);

		void LoadImages(const std::vector<std::string>& filenames);
		void FreeImages();

		void InitTexture();
	};

}

#endif // !DIRECTX11_TEXTURE3D_H_

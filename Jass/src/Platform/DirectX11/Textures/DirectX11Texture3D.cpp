#include "jasspch.h"
#include "DirectX11Texture3D.h"

#include "stb_image.h"

namespace Jass {

	ComPtr<ID3D11SamplerState> DirectX11Texture3D::s_samplerState = nullptr;

	bool DirectX11Texture3D::s_samplerStateBound = false;

	void DirectX11Texture3D::InitSamplerState()
	{
		if (s_samplerState == nullptr)
		{
			D3D11_SAMPLER_DESC sDesc = {};
			sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

			auto device = DirectX11Graphics::Get().GetDevice();
			device->CreateSamplerState(&sDesc, &s_samplerState);
		}
	}

	DirectX11Texture3D::DirectX11Texture3D(const std::vector<std::string>& filenames)
	{
		LoadImages(filenames);
		InitTexture();
		FreeImages();
		InitSamplerState();
	}

	DirectX11Texture3D::~DirectX11Texture3D()
	{
	}

	void DirectX11Texture3D::Bind(unsigned int slot) const
	{
		auto deviceContext = DirectX11Graphics::Get().GetDeviceContext();
		deviceContext->PSSetShaderResources(slot, 1u, m_shaderResourceView.GetAddressOf());

		JASS_CORE_ASSERT(s_samplerState != nullptr, "The sampler state is nullptr");
		deviceContext->PSSetSamplers(0u, 1u, s_samplerState.GetAddressOf());
		// There is only one sampler state for every texture, that should be bound to the slot 1
		if (!s_samplerStateBound) {
			deviceContext->PSSetSamplers(1u, 1u, s_samplerState.GetAddressOf());
			s_samplerStateBound = true;
		}
	}

	void DirectX11Texture3D::SetData(const void* data, unsigned int size)
	{
	}

	bool DirectX11Texture3D::IsEqual(const ITexture& other) const
	{
		auto& otherTexture = static_cast<const DirectX11Texture3D&>(other);
		return this->m_shaderResourceView.Get() == otherTexture.m_shaderResourceView.Get();
	}

	DirectX11Texture3D::ImageData DirectX11Texture3D::ProcessImage(const std::string& filename)
	{
		int width, height, channels;
		stbi_uc* data = nullptr;
		// Load the image in the way OpenGL expects
		{
			JASS_PROFILE_SCOPE("stbi Load");
			stbi_set_flip_vertically_on_load(1);
			data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
		}

		return { data, (unsigned int)width, (unsigned int)height, (unsigned int)channels };
	}

	void DirectX11Texture3D::FreeImage(ImageData image)
	{
		stbi_image_free(image.Data);
	}

	void DirectX11Texture3D::LoadImages(const std::vector<std::string>& filenames)
	{
		JASS_CORE_ASSERT(filenames.size() <= 6, "Maximum number of textures is six");

		for (unsigned int i = 0; i < filenames.size(); i++)
		{
			auto imageData = ProcessImage(filenames[i]);

			if (imageData.Data) {
				m_images.push_back(imageData);
			}
			else {
				JASS_CORE_WARN("Failed to load the image with filepath: {0}", filenames[i]);
			}
		}
	}

	void DirectX11Texture3D::FreeImages()
	{
		for (const auto& image : m_images)
		{
			FreeImage(image);
		}
	}

	void DirectX11Texture3D::InitTexture()
	{
		JASS_CORE_ASSERT(m_images.size() == 6, "Invalid number of textures");
		
		m_width = m_images[0].Width;
		m_height = m_images[1].Height;

		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = m_width;
		tDesc.Height = m_height;
		tDesc.MipLevels = 1u;
		tDesc.ArraySize = 6u;
		tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tDesc.SampleDesc.Count = 1u;
		tDesc.SampleDesc.Quality = 0u;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.CPUAccessFlags = 0u;
		tDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		D3D11_SUBRESOURCE_DATA sd[6] = {};
		unsigned int i = 0;
		for (const auto& image : m_images)
		{
			sd[i].pSysMem = image.Data;
			sd[i].SysMemPitch = image.Width * 4u; // 4u is RGBA Channels
			sd[i].SysMemSlicePitch = 0u;
			
			i++;
		}

		ComPtr<ID3D11Texture2D> texture;

		auto device = DirectX11Graphics::Get().GetDevice();
		device->CreateTexture2D(&tDesc, sd, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = tDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Texture2D.MipLevels = 1u;
		srvDesc.Texture2D.MostDetailedMip = 0u;

		device->CreateShaderResourceView(texture.Get(), &srvDesc, &m_shaderResourceView);
	}

}

#include "jasspch.h"
#include "DirectX11Texture2D.h"

#include "stb_image.h"

namespace Jass {

	ComPtr<ID3D11SamplerState> DirectX11Texture2D::s_samplerState = nullptr;

	bool DirectX11Texture2D::s_samplerStateBound = false;

	void DirectX11Texture2D::InitSamplerState()
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

	DirectX11Texture2D::DirectX11Texture2D(unsigned int width, unsigned int height)
		:m_width(width), m_height(height)
	{
	}

	DirectX11Texture2D::DirectX11Texture2D(const std::string& filepath)
		:m_filepath(filepath)
	{
		int width, height, channels;
		stbi_uc* data = nullptr;
		{
			JASS_PROFILE_SCOPE("stbi Load");
			stbi_set_flip_vertically_on_load(1);
			data = stbi_load(filepath.c_str(), &width, &height, &channels, 4u);
		}

		if (data) {
			m_width = (unsigned int)width;
			m_height = (unsigned int)height;

			InitTexture(data);
			InitSamplerState();

			stbi_image_free(data);
		}
		else {
			JASS_CORE_WARN("Failed to load the image with filepath: {0}", filepath);
		}
	}

	void DirectX11Texture2D::Bind(unsigned int slot) const
	{
		auto deviceContext = DirectX11Graphics::Get().GetDeviceContext();
		deviceContext->PSSetShaderResources(slot, 1u, m_shaderResourceView.GetAddressOf());
		
		JASS_CORE_ASSERT(s_samplerState != nullptr, "The sampler state is nullptr");
		deviceContext->PSSetSamplers(0u, 1u, s_samplerState.GetAddressOf());
		// There is only one sampler state for every texture, that should be bound to the slot 0
		if (!s_samplerStateBound) {
			deviceContext->PSSetSamplers(0u, 1u, s_samplerState.GetAddressOf());
			s_samplerStateBound = true;
		}
	}

	void DirectX11Texture2D::SetData(const void* data, unsigned int size)
	{
		JASS_CORE_ASSERT(size == m_width * m_height * 4u,
			"Data size do not match with the texture properties");

		InitTexture(data);
		InitSamplerState();
	}

	bool DirectX11Texture2D::IsEqual(const ITexture& other) const
	{
		auto& otherTexture = static_cast<const DirectX11Texture2D&>(other);
		return this->m_shaderResourceView.Get() == otherTexture.m_shaderResourceView.Get();
	}

	void DirectX11Texture2D::InitTexture(const void* texData)
	{
		D3D11_TEXTURE2D_DESC tDesc = {};
		tDesc.Width = m_width;
		tDesc.Height = m_height;
		tDesc.MipLevels = 1u;
		tDesc.ArraySize = 1u;
		tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		tDesc.SampleDesc.Count = 1u;
		tDesc.SampleDesc.Quality = 0u;
		tDesc.Usage = D3D11_USAGE_DEFAULT;
		tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tDesc.CPUAccessFlags = 0u;
		tDesc.MiscFlags = 0u;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = texData;
		sd.SysMemPitch = m_width * 4u; // 4u -> RGBA channels

		ComPtr<ID3D11Texture2D> texture;

		auto device = DirectX11Graphics::Get().GetDevice();
		device->CreateTexture2D(&tDesc, &sd, &texture);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = tDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1u;
		srvDesc.Texture2D.MostDetailedMip = 0u;

		device->CreateShaderResourceView(texture.Get(), &srvDesc, &m_shaderResourceView);
	}

}

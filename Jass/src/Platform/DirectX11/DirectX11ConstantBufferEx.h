#ifndef DIRECTX_11_CONSTANT_BUFFER_EX_H
#define DIRECTX_11_CONSTANT_BUFFER_EX_H

#include "DirectX11Graphics.h"
#include "DynamicConstantBuffer/Buffer.h"

namespace Jass {

	enum class Destination
	{
		VertexShader,
		PixelShader
	};

	class DirectX11ConstantBufferEx
	{
	public:
		DirectX11ConstantBufferEx(Dcb::RawLayout&& layout, Destination destination, unsigned int slot);

		void Bind() const;

		Dcb::Buffer& GetBuffer() { return m_buffer; }

	private:
		ComPtr<ID3D11Buffer> m_constantBuffer;

		Dcb::Buffer m_buffer;

		Destination m_destination;
		unsigned int m_slot;

		void Update(ComPtr<ID3D11DeviceContext> deviceContext) const;
	};

}

#endif // !DIRECTX_11_CONSTANT_BUFFER_EX_H

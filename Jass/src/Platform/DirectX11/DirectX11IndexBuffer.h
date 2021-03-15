#ifndef DIRECTX11_INDEX_BUFFER_H
#define DIRECTX11_INDEX_BUFFER_H

#include "Jass/Renderer/IndexBuffer.h"

#include "DirectX11Graphics.h"

namespace Jass {

	class JASS_API DirectX11IndexBuffer : public IndexBuffer {

	public:
		DirectX11IndexBuffer(const IndexBufferConfig& config);

		virtual ~DirectX11IndexBuffer() = default;

		virtual	inline unsigned int GetCount() const override { return m_count; }

		virtual void Bind() const override;
		virtual void Unbind() const override;

		ComPtr<ID3D11Buffer> GetDirectX11Buffer() const { return m_indexBuffer; }

	private:
		ComPtr<ID3D11Buffer> m_indexBuffer;

		unsigned int m_count;

	};

}

#endif // !JASS_DIRECTX11_INDEX_BUFFER_H

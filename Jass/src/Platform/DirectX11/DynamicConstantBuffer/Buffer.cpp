#include "jasspch.h"
#include "Buffer.h"

namespace Dcb {

	Buffer::Buffer(RawLayout&& layout)
		:Buffer(LayoutCodex::Resolve(std::move(layout)))
	{
	}

	Buffer::Buffer(const FixedLayout& layout)
		:m_layoutRoot(layout.ShareRoot()), m_bytes(m_layoutRoot->GetOffsetEnd())
	{
	}

	Buffer::Buffer(FixedLayout&& layout)
		:m_layoutRoot(layout.RenounceRoot()), m_bytes(m_layoutRoot->GetOffsetEnd())
	{
	}

	Buffer::Buffer(const Buffer& buffer)
		:m_layoutRoot(buffer.m_layoutRoot), m_bytes(buffer.m_bytes)
	{
	}

	Buffer::Buffer(Buffer&& buffer) noexcept
		:m_layoutRoot(std::move(buffer.m_layoutRoot)), m_bytes(std::move(buffer.m_bytes))
	{
	}

	ElementRef Buffer::operator[](const std::string& key)
	{
		return { &(*m_layoutRoot)[key], m_bytes.data(), 0u };
	}

	ConstElementRef Buffer::operator[](const std::string& key) const
	{
		//return const_cast<Buffer&>(*this)[key];
		return { &(*m_layoutRoot)[key], m_bytes.data(), 0u };
	}

	void Buffer::CopyFrom(const Buffer&& buffer)
	{
		assert(&GetRootLayoutElement() == &buffer.GetRootLayoutElement());
		std::copy(buffer.m_bytes.begin(), buffer.m_bytes.end(), m_bytes.begin());
	}

}

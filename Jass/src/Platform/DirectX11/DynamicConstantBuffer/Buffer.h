#ifndef DCB_BUFFER_H
#define DCB_BUFFER_H

#include "ElementRef.h"
#include "LayoutCodex.h"

namespace Dcb {

	class Buffer
	{
	public:
		Buffer(RawLayout&& layout);
		Buffer(const FixedLayout& layout);
		Buffer(FixedLayout&& layout);
		Buffer(const Buffer& buffer);
		Buffer(Buffer&& buffer) noexcept;

		ElementRef operator[](const std::string& key);
		ConstElementRef operator[](const std::string& key) const;

		const char* GetData() const { return m_bytes.data(); }

		size_t GetSizeInBytes() const { return m_bytes.size(); }

		const LayoutElement& GetRootLayoutElement() const { return *m_layoutRoot; }
		std::shared_ptr<LayoutElement> ShareRootLayoutElement() const { return m_layoutRoot; }

		void CopyFrom(const Buffer&& buffer);

	private:
		std::shared_ptr<LayoutElement> m_layoutRoot;
		std::vector<char> m_bytes;
	};

}

#endif // !DCB_BUFFER_H

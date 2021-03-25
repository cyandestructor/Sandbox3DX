#include "jasspch.h"
#include "ConstElementRef.h"

namespace Dcb {

	bool ConstElementRef::Exists() const
	{
		return m_layout->Exists();
	}

	ConstElementRef ConstElementRef::operator[](size_t index) const
	{
		const auto indexingData = m_layout->CalculateIndexingOffset(m_offset, index);
		return { indexingData.second, m_bytes, indexingData.first };
	}

	ConstElementRef::Ptr ConstElementRef::operator&() const
	{
		return Ptr(this);
	}

	ConstElementRef::ConstElementRef(const LayoutElement* layout, const char* bytes, size_t offset)
		:m_layout(layout), m_bytes(bytes), m_offset(offset)
	{
	}

	ConstElementRef::Ptr::Ptr(const ConstElementRef* ref)
		:m_ref(ref)
	{
	}

}

#include "jasspch.h"
#include "ElementRef.h"

namespace Dcb {
	
	ElementRef::operator ConstElementRef() const
	{
		return { m_layout, m_bytes, m_offset };
	}

	bool ElementRef::Exists() const
	{
		return m_layout->Exists();
	}

	ElementRef ElementRef::operator[](const std::string& key) const
	{
		return { &(*m_layout)[key], m_bytes, m_offset };
	}

	ElementRef ElementRef::operator[](size_t index) const
	{
		const auto indexingData = m_layout->CalculateIndexingOffset(m_offset, index);
		return { indexingData.second, m_bytes, indexingData.first };
	}

	ElementRef::Ptr ElementRef::operator&() const
	{
		return Ptr(const_cast<ElementRef*>(this));
	}

	ElementRef::ElementRef(const LayoutElement* layout, char* bytes, size_t offset)
		:m_layout(layout), m_bytes(bytes), m_offset(offset)
	{
	}

	ElementRef::Ptr::Ptr(ElementRef* ref)
		:m_ref(ref)
	{
	}

}

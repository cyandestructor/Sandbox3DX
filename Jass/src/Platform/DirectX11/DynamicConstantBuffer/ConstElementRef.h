#ifndef DCB_CONST_ELEMENT_REF_H
#define DCB_CONST_ELEMENT_REF_H

#include "LayoutElement.h"

namespace Dcb {

	class ConstElementRef
	{
		friend class Buffer;
		friend class ElementRef;
	public:
		class Ptr
		{
			friend class ConstElementRef;
		public:
			// Conversion to get read-only pointers to supported System Types
			template<typename T>
			operator const T* () const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported system type used in conversion");
				return &static_cast<const T&>(*ref);
			}

		private:
			const ConstElementRef* m_ref;

			Ptr(const ConstElementRef* ref);

		};

		bool Exists() const;

		ConstElementRef operator[](const std::string& key) const;
		ConstElementRef operator[](size_t index) const;

		Ptr operator&() const;

		template<typename T>
		operator const T& () const
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported system type used in conversion");
			return *reinterpret_cast<const T*>(m_bytes + m_offset + m_layout->Resolve<T>());
		}

	private:
		size_t m_offset;
		const LayoutElement* m_layout;
		const char* m_bytes;

		ConstElementRef(const LayoutElement* layout, const char* bytes, size_t offset);
	};

}

#endif // !DCB_CONST_ELEMENT_REF_H

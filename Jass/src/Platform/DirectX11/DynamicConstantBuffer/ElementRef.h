#ifndef DCB_ELEMENT_REF_H
#define DCB_ELEMENT_REF_H

#include "ConstElementRef.h"

namespace Dcb {

	class ElementRef
	{
		friend class Buffer;
	public:
		class Ptr
		{
			friend class ElementRef;
		public:
			// Conversion to get read-only pointers to supported System Types
			template<typename T>
			operator T* () const
			{
				static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported system type used in conversion");
				return &static_cast<T&>(*ref);
			}

		private:
			ElementRef* m_ref;

			Ptr(ElementRef* ref);

		};

		operator ConstElementRef() const;

		bool Exists() const;

		ElementRef operator[](const std::string& key) const;
		ElementRef operator[](size_t index) const;

		Ptr operator&() const;

		template<typename S>
		bool SetIfExists(const S& value)
		{
			if (Exists())
			{
				*this = value;
				return true;
			}

			return false;
		}

		template<typename T>
		operator T& () const
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported system type used in conversion");
			return *reinterpret_cast<T*>(m_bytes + m_offset + m_layout->Resolve<T>());
		}

		template<typename T>
		T& operator=(const T& value) const
		{
			static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported system type used in assignment");
			return static_cast<T&>(*this) = value;
		}

	private:
		size_t m_offset;
		const LayoutElement* m_layout;
		char* m_bytes;

		ElementRef(const LayoutElement* layout, char* bytes, size_t offset);
	};

}

#endif // !DCB_ELEMENT_REF_H

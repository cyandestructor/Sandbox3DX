#ifndef DCB_LAYOUT_ELEMENT_H
#define DCB_LAYOUT_ELEMENT_H

#include "ElementTypes.h"
#include <optional>

namespace Dcb {

	class LayoutElement
	{
		friend struct ExtraData;
		friend class RawLayout;
	public:
		std::string GetSignature() const;
		bool Exists() const { return m_type != Type::Empty; }

		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const;

		// Only for structs
		LayoutElement& operator[](const std::string& key);
		const LayoutElement& operator[](const std::string& key) const;
		LayoutElement& Add(Type type, const std::string& name); // Add layout element to a struct

		// Only for arrays
		LayoutElement& ArrayType();
		const LayoutElement& ArrayType() const;
		LayoutElement& Set(Type type, size_t size); // Set the type and size of an array

		// Only after finalization
		size_t GetOffsetBegin() const;
		size_t GetOffsetEnd() const;
		size_t GetSizeInBytes() const;

		template<typename T>
		size_t Resolve() const
		{
			switch (m_type)
			{
			#define X(element) \
			case Type::##element: \
				assert(typeid(Map<Type::##element>::SysType) == typeid(T)); \
				return *m_offset;
			LEAF_ELEMENT_TYPES
			#undef X
			default:
				assert("Tried to resolve a non-leaf element" && false);
				return 0u;
			}
		}

	private:
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};

		std::optional<size_t> m_offset;
		Type m_type = Type::Empty;
		std::unique_ptr<ExtraDataBase> m_extraData;

		static LayoutElement& GetEmpty();

		static size_t AdvanceToBoundary(size_t offset);
		static bool CrossesBoundary(size_t offset, size_t size);
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size);

		static bool ValidateSymbolName(const std::string& name);

		LayoutElement() = default;
		LayoutElement(Type type);

		size_t Finalize(size_t offsetIn);
		size_t FinalizeStruct(size_t offsetIn);
		size_t FinalizeArray(size_t offsetIn);

		std::string GetStructSignature() const;
		std::string GetArraySignature() const;
	};
}

#endif // !DCB_LAYOUT_ELEMENT_H

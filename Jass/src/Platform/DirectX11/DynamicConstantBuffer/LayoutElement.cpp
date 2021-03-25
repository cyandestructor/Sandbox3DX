#include "jasspch.h"
#include "LayoutElement.h"

namespace Dcb {

	struct ExtraData
	{
		struct Struct : LayoutElement::ExtraDataBase
		{
			std::vector<std::pair<std::string, LayoutElement>> LayoutElements;
		};

		struct Array : LayoutElement::ExtraDataBase
		{
			std::optional<LayoutElement> LayoutElement;
			size_t ElementSize = 0u;
			size_t Size = 0u;
		};
	};

	std::string LayoutElement::GetSignature() const
	{
		switch (m_type)
		{
		#define X(element) case Type::##element: return Map<Type::##element>::Code;
		LEAF_ELEMENT_TYPES
		#undef X
		case Type::Struct:
			return GetStructSignature();
		case Type::Array:
			return GetArraySignature();
		default:
			assert("Invalid type in signature generation" && false);
			return "??";
		}
	}

	std::pair<size_t, const LayoutElement*> LayoutElement::CalculateIndexingOffset(size_t offset, size_t index) const
	{
		assert("Indexing into a non-array element" && m_type == Type::Array);
		
		const auto& arrayData = static_cast<ExtraData::Array&>(*m_extraData);
		assert(index < arrayData.Size);

		return { offset + arrayData.ElementSize * index, &(*arrayData.LayoutElement) };
	}

	LayoutElement& LayoutElement::operator[](const std::string& key)
	{
		assert("The Element is not a struct" && m_type == Type::Struct);

		for (auto& member : static_cast<ExtraData::Struct&>(*m_extraData).LayoutElements)
		{
			if (member.first == key)
				return member.second;
		}

		return GetEmpty();
	}

	const LayoutElement& LayoutElement::operator[](const std::string& key) const
	{
		return const_cast<LayoutElement&>(*this)[key];
	}

	LayoutElement& LayoutElement::Add(Type type, const std::string& name)
	{
		assert("Tried to add to a non-struct element" && m_type == Type::Struct);
		assert("Invalid symbol name" && ValidateSymbolName(name));

		auto& structData = static_cast<ExtraData::Struct&>(*m_extraData);
		// Check for duplicates
		for (auto& member : structData.LayoutElements)
		{
			if (member.first == name)
			{
				assert("Duplicate element in the struct" && false);
			}
		}

		structData.LayoutElements.emplace_back(name, LayoutElement(type));

		return *this;
	}

	LayoutElement& LayoutElement::ArrayType()
	{
		assert("Tried to get array from a non-array element" && m_type == Type::Array);

		return *static_cast<ExtraData::Array&>(*m_extraData).LayoutElement;
	}

	const LayoutElement& LayoutElement::ArrayType() const
	{
		return const_cast<LayoutElement&>(*this).ArrayType();
	}

	LayoutElement& LayoutElement::Set(Type type, size_t size)
	{
		assert("Tried to set an non-array element" && m_type == Type::Array);
		assert(size > 0u);
		auto& arrayData = static_cast<ExtraData::Array&>(*m_extraData);
		arrayData.LayoutElement = LayoutElement(type);
		arrayData.Size = size;

		return *this;
	}

	size_t LayoutElement::GetOffsetBegin() const
	{
		return *m_offset;
	}

	size_t LayoutElement::GetOffsetEnd() const
	{
		switch (m_type)
		{
		#define X(element) case Type::##element: return *m_offset + Map<Type::##element>::GpuSize;
		LEAF_ELEMENT_TYPES
		#undef X
		case Type::Struct:
		{
			const auto& data = static_cast<ExtraData::Struct&>(*m_extraData);
			// return the offset of the last element of the struct
			return AdvanceToBoundary(data.LayoutElements.back().second.GetOffsetEnd());
		}
		case Type::Array:
		{
			const auto& data = static_cast<ExtraData::Array&>(*m_extraData);
			return *m_offset + AdvanceToBoundary(data.LayoutElement->GetSizeInBytes()) * data.Size;
		}
		default:
			assert("Tried to get offset of an empty or invalid element" && false);
			return 0u;
		}
	}

	size_t LayoutElement::GetSizeInBytes() const
	{
		auto end = GetOffsetEnd();
		auto begin = GetOffsetBegin();
		auto result = end - begin;
		return result;
	}

	LayoutElement& LayoutElement::GetEmpty()
	{
		static LayoutElement empty{};
		return empty;
	}

	size_t LayoutElement::AdvanceToBoundary(size_t offset)
	{
		// Constant buffer should be multiple of 16 bytes
		return offset + (16u - offset % 16u) % 16u;
	}

	bool LayoutElement::CrossesBoundary(size_t offset, size_t size)
	{
		const auto end = offset + size;
		const auto pageStart = offset / 16u;
		const auto pageEnd = end / 16u;
		return (pageStart != pageEnd && end % 16u != 0u) || size > 16u;
	}

	size_t LayoutElement::AdvanceIfCrossesBoundary(size_t offset, size_t size)
	{
		return CrossesBoundary(offset, size) ? AdvanceToBoundary(offset) : offset;
	}

	bool LayoutElement::ValidateSymbolName(const std::string& name)
	{
		return !name.empty() && !std::isdigit(name.front()) &&
			std::all_of(name.begin(), name.end(), [](char c) {
				return std::isalnum(c) || c == '_';
			});
	}

	LayoutElement::LayoutElement(Type type)
		:m_type(type)
	{
		assert(type != Type::Empty);
		if (type == Type::Struct) {
			m_extraData = std::unique_ptr<ExtraData::Struct>{ new ExtraData::Struct() };
		}
		else if (type == Type::Array) {
			m_extraData = std::unique_ptr<ExtraData::Array>{ new ExtraData::Array() };
		}
	}

	size_t LayoutElement::Finalize(size_t offsetIn)
	{
		switch (m_type)
		{
		#define X(element) \
		case Type::##element: \
			m_offset = AdvanceIfCrossesBoundary(offsetIn, Map<Type::##element>::GpuSize); \
			return *m_offset + Map<Type::##element>::GpuSize;
		LEAF_ELEMENT_TYPES
		#undef X
		case Type::Struct:
			return FinalizeStruct(offsetIn);
		case Type::Array:
			return FinalizeArray(offsetIn);
		default:
			assert("Invalid element type" && false);
			return 0u;
		}
	}

	size_t LayoutElement::FinalizeStruct(size_t offsetIn)
	{
		auto& structData = static_cast<ExtraData::Struct&>(*m_extraData);
		assert(structData.LayoutElements.size() > 0u);

		m_offset = AdvanceToBoundary(offsetIn);

		auto nextOffset = *m_offset;
		for (auto& member : structData.LayoutElements)
		{
			nextOffset = member.second.Finalize(nextOffset);
		}

		return nextOffset;
	}

	size_t LayoutElement::FinalizeArray(size_t offsetIn)
	{
		auto& arrayData = static_cast<ExtraData::Array&>(*m_extraData);
		assert(arrayData.Size > 0u);
		m_offset = AdvanceToBoundary(offsetIn);
		arrayData.LayoutElement->Finalize(*m_offset);
		arrayData.ElementSize = AdvanceToBoundary(arrayData.LayoutElement->GetSizeInBytes());

		return GetOffsetEnd();
	}

	std::string LayoutElement::GetStructSignature() const
	{
		std::string signature = "ST{";
		for (const auto& member : static_cast<ExtraData::Struct&>(*m_extraData).LayoutElements)
		{
			signature += member.first + ":" + member.second.GetSignature() + ";";
		}
		signature += "}";

		return signature;
	}

	std::string LayoutElement::GetArraySignature() const
	{
		const auto& data = static_cast<ExtraData::Array&>(*m_extraData);
		return "AR:" + std::to_string(data.Size) + "{" + data.LayoutElement->GetSignature() + "}";
	}

}

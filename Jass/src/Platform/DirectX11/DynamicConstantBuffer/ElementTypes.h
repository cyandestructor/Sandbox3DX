#ifndef DCB_ELEMENT_TYPES_H
#define DCB_ELEMENT_TYPES_H

#include "Jass/JMath/JMath.h"

// This is a useful matrix that allow us to add new types and automatically update
// the rest of the code with that type
// X is a macro that should be defined as: what I want to do with every type in a certain piece of code
#define LEAF_ELEMENT_TYPES \
	X(Float) \
	X(Float2) \
	X(Float3) \
	X(Float4) \
	X(Matrix3) \
	X(Matrix4) \
	X(Boolean) \
	X(Integer)

namespace Dcb {

	enum class Type
	{
		#define X(element) element,
		LEAF_ELEMENT_TYPES
		#undef X
		Struct,
		Array,
		Empty
	};

	// Static map of attributes for each leaf type
	template<Type type>
	struct Map
	{
		static constexpr bool Valid = false;
	};

	template<>
	struct Map<Type::Float>
	{
		using SysType = float; // Type used on the cpu
		static constexpr size_t GpuSize = sizeof(SysType); // Size of the type used on the gpu
		static constexpr const char* Code = "F1"; // Code used for layout signature
		static constexpr bool Valid = true; // Metaprogramming flag to check validity of the map with this type
	};

	template<>
	struct Map<Type::Float2>
	{
		using SysType = Jass::JVec2;
		static constexpr size_t GpuSize = sizeof(SysType);
		static constexpr const char* Code = "F2";
		static constexpr bool Valid = true;
	};

	template<>
	struct Map<Type::Float3>
	{
		using SysType = Jass::JVec3;
		static constexpr size_t GpuSize = sizeof(SysType);
		static constexpr const char* Code = "F3";
		static constexpr bool Valid = true;
	};

	template<>
	struct Map<Type::Float4>
	{
		using SysType = Jass::JVec4;
		static constexpr size_t GpuSize = sizeof(SysType);
		static constexpr const char* Code = "F4";
		static constexpr bool Valid = true;
	};

	template<>
	struct Map<Type::Matrix3>
	{
		using SysType = Jass::JMat3;
		static constexpr size_t GpuSize = sizeof(SysType);
		static constexpr const char* Code = "M3";
		static constexpr bool Valid = true;
	};

	template<>
	struct Map<Type::Matrix4>
	{
		using SysType = Jass::JMat4;
		static constexpr size_t GpuSize = sizeof(SysType);
		static constexpr const char* Code = "M4";
		static constexpr bool Valid = true;
	};

	template<>
	struct Map<Type::Boolean>
	{
		using SysType = bool;
		static constexpr size_t GpuSize = 4u;
		static constexpr const char* Code = "BL";
		static constexpr bool Valid = true;
	};

	template<>
	struct Map<Type::Integer>
	{
		using SysType = int;
		static constexpr size_t GpuSize = sizeof(SysType);
		static constexpr const char* Code = "IN";
		static constexpr bool Valid = true;
	};

	// Ensure that every leaf has an entry in the static attribute map above
	#define X(element) static_assert(Map<Type::##element>::Valid, "Missing map implementation for " #element);
	LEAF_ELEMENT_TYPES
	#undef X

	// Enable lookup from SysType to Leaf Type
	template<typename T>
	struct ReverseMap
	{
		static constexpr bool Valid = false;
	};

	#define X(element) \
	template<> \
	struct ReverseMap<typename Map<Type::##element>::SysType> { \
		static constexpr Type Type = Type::##element; \
		static constexpr bool Valid = true; \
	};
	LEAF_ELEMENT_TYPES
	#undef X
}

#endif // !DCB_ELEMENT_TYPES_H

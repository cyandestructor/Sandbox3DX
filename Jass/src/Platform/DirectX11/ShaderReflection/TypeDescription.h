#ifndef TYPE_DESCRIPTION_H
#define TYPE_DESCRIPTION_H

#include <d3d11shader.h>

namespace Jass {

	enum class ShaderVariableType
	{
		Null = 0,
		Bool,
		Int,
		Float,
		Float2,
		Float3,
		Float4,
		Matrix3,
		Matrix4,
		Struct
	};

	class TypeDescription
	{
	public:
		static ShaderVariableType ToShaderVariableType(const D3D11_SHADER_TYPE_DESC& typeDesc);
		
		TypeDescription() = default;
		TypeDescription(ID3D11ShaderReflectionType* typeReflection) { Set(typeReflection); }
		
		const D3D11_SHADER_TYPE_DESC& GetDirectX11Description() const { return m_typeDescription; }

		void Set(ID3D11ShaderReflectionType* typeReflection);

		bool IsArray() const { return m_typeDescription.Elements > 0u; }
		unsigned int GetElementsCount() const { return m_typeDescription.Elements; }

		const std::vector<TypeDescription>& GetMembers() const { return m_members; }
		const std::string& GetMemberName() const { return m_memberName; }

		ShaderVariableType GetType() const { return m_type; }

	private:
		ID3D11ShaderReflectionType* m_typeReflection = nullptr;
		D3D11_SHADER_TYPE_DESC m_typeDescription = {};
		ShaderVariableType m_type = ShaderVariableType::Null;

		std::string m_memberName;
		std::vector<TypeDescription> m_members;
		
		TypeDescription(ID3D11ShaderReflectionType* typeReflection, const std::string& name)
			:m_memberName(name)
		{
			Set(typeReflection);
		}

		void GetMembers(ID3D11ShaderReflectionType* typeReflection, const D3D11_SHADER_TYPE_DESC& typeDescription);
	};

}

#endif // !TYPE_DESCRIPTION_H

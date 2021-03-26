#include "jasspch.h"
#include "TypeDescription.h"

namespace Jass {

	ShaderVariableType TypeDescription::ToShaderVariableType(const D3D11_SHADER_TYPE_DESC& typeDesc)
	{
		ShaderVariableType type = ShaderVariableType::Null;

		switch (typeDesc.Class)
		{
		case D3D_SVC_STRUCT:
			type = ShaderVariableType::Struct;
			break;
		case D3D_SVC_MATRIX_ROWS:
		case D3D_SVC_MATRIX_COLUMNS:
			if (typeDesc.Rows == 3u && typeDesc.Columns == 3u) {
				type = ShaderVariableType::Matrix3;
			}
			else if (typeDesc.Rows == 4u && typeDesc.Columns == 4u) {
				type = ShaderVariableType::Matrix4;
			}
			break;
		case D3D_SVC_VECTOR:
			if (typeDesc.Type == D3D_SVT_FLOAT) {
				switch (typeDesc.Columns)
				{
				case 2u:
					type = ShaderVariableType::Float2;
					break;
				case 3u:
					type = ShaderVariableType::Float3;
					break;
				case 4u:
					type = ShaderVariableType::Float4;
					break;
				}
			}
			break;
		case D3D_SVC_SCALAR:
			switch (typeDesc.Type)
			{
			case D3D_SVT_FLOAT:
				type = ShaderVariableType::Float;
				break;
			case D3D_SVT_INT:
				type = ShaderVariableType::Int;
				break;
			case D3D_SVT_BOOL:
				type = ShaderVariableType::Bool;
				break;
			}
			break;
		}

		return type;
	}

	void TypeDescription::Set(ID3D11ShaderReflectionType* typeReflection)
	{
		JASS_CORE_ASSERT(typeReflection != nullptr, "The type reflection is nullptr");
		m_typeReflection = typeReflection;

		m_typeReflection->GetDesc(&m_typeDescription);

		m_type = ToShaderVariableType(m_typeDescription);

		if (m_type == ShaderVariableType::Struct) {
			GetMembers(typeReflection, m_typeDescription);
		}
	}

	void TypeDescription::GetMembers(ID3D11ShaderReflectionType* typeReflection, const D3D11_SHADER_TYPE_DESC& typeDescription)
	{
		unsigned int memberCount = typeDescription.Members;
		m_members.reserve(memberCount);

		for (unsigned int i = 0; i < memberCount; i++) {
			std::string memberName = typeReflection->GetMemberTypeName(i);
			m_members.emplace_back(TypeDescription(typeReflection->GetMemberTypeByIndex(i), memberName));
		}
	}

}

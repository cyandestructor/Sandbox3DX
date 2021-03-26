#ifndef VARIABLE_DESCRIPTION_H
#define VARIABLE_DESCRIPTION_H

#include "TypeDescription.h"

namespace Jass {

	class VariableDescription
	{
	public:
		VariableDescription(ID3D11ShaderReflectionVariable* variableReflection);

		std::string GetName() const { return m_description.Name; }
		
		const TypeDescription& GetTypeDescription() const { return m_typeDescription; }
		ShaderVariableType GetType() const { return m_typeDescription.GetType(); }

		bool IsArray() const { return m_typeDescription.IsArray(); }
		unsigned int GetElementsCount() const { return m_typeDescription.GetElementsCount(); }
		
		const std::vector<TypeDescription>& GetMembers() const { return m_typeDescription.GetMembers(); }

	private:
		ID3D11ShaderReflectionVariable* m_variableReflection = nullptr;
		D3D11_SHADER_VARIABLE_DESC m_description = {};

		TypeDescription m_typeDescription;
	};

}

#endif // !VARIABLE_DESCRIPTION_H

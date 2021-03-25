#include "jasspch.h"
#include "VariableDescription.h"

namespace Jass {

	VariableDescription::VariableDescription(ID3D11ShaderReflectionVariable* variableReflection)
		:m_variableReflection(variableReflection)
	{
		JASS_CORE_ASSERT(m_variableReflection != nullptr, "Variable reflection is nullptr");

		m_variableReflection->GetDesc(&m_description);

		ID3D11ShaderReflectionType* typeReflection = m_variableReflection->GetType();
		m_typeDescription.Set(typeReflection);
	}

}

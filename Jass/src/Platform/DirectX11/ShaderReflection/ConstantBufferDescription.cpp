#include "jasspch.h"
#include "ConstantBufferDescription.h"

namespace Jass {

	ConstantBufferDescription::ConstantBufferDescription(ID3D11ShaderReflectionConstantBuffer* bufferReflection)
		:m_bufferReflection(bufferReflection)
	{
		JASS_CORE_ASSERT(m_bufferReflection != nullptr, "The buffer reflection is nullptr");

		m_bufferReflection->GetDesc(&m_description);

		unsigned int variableCount = m_description.Variables;
		m_variables.reserve(variableCount);

		for (unsigned int i = 0; i < variableCount; i++)
		{
			m_variables.emplace_back(m_bufferReflection->GetVariableByIndex(i));
		}
	}

}

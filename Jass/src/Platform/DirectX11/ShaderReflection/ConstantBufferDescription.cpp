#include "jasspch.h"
#include "ConstantBufferDescription.h"

namespace Jass {

	ConstantBufferDescription::ConstantBufferDescription(ID3D11ShaderReflectionConstantBuffer* bufferReflection,
		const D3D11_SHADER_BUFFER_DESC& description,
		unsigned int slot)
		:m_bufferReflection(bufferReflection), m_description(description), m_slot(slot)
	{
		JASS_CORE_ASSERT(m_bufferReflection != nullptr, "The buffer reflection is nullptr");
		
		GetVariables();
	}

	ConstantBufferDescription::ConstantBufferDescription(ID3D11ShaderReflectionConstantBuffer* bufferReflection, unsigned int slot)
		:m_bufferReflection(bufferReflection), m_slot(slot)
	{
		JASS_CORE_ASSERT(m_bufferReflection != nullptr, "The buffer reflection is nullptr");

		m_bufferReflection->GetDesc(&m_description);

		GetVariables();
	}

	void ConstantBufferDescription::GetVariables()
	{
		unsigned int variableCount = m_description.Variables;
		m_variables.reserve(variableCount);

		for (unsigned int i = 0; i < variableCount; i++)
		{
			m_variables.emplace_back(m_bufferReflection->GetVariableByIndex(i));
		}
	}

}

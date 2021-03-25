#include "jasspch.h"
#include "ShaderDescription.h"

namespace Jass {

	ShaderDescription::ShaderDescription(ID3D11ShaderReflection* reflection)
		:m_reflection(reflection)
	{
		JASS_CORE_ASSERT(m_reflection != nullptr, "The reflection is nullptr");

		m_reflection->GetDesc(&m_description);

		unsigned int constantBuffersCount = m_description.ConstantBuffers;
		m_constantBuffers.reserve(constantBuffersCount);

		for (unsigned int i = 0; i < constantBuffersCount; i++)
		{
			m_constantBuffers.emplace_back(m_reflection->GetConstantBufferByIndex(i));
		}
	}

}

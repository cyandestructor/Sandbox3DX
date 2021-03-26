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
			ID3D11ShaderReflectionConstantBuffer* constantBufferReflection = m_reflection->GetConstantBufferByIndex(i);
			
			D3D11_SHADER_BUFFER_DESC bDesc = {};
			constantBufferReflection->GetDesc(&bDesc);

			D3D11_SHADER_INPUT_BIND_DESC ibDesc = {};
			HRESULT hr = m_reflection->GetResourceBindingDescByName(bDesc.Name ? bDesc.Name : "", &ibDesc);

			JASS_CORE_ASSERT(SUCCEEDED(hr), "Get resource binding description failed");

			unsigned int slot = ibDesc.BindPoint;

			m_constantBuffers.emplace_back(constantBufferReflection, bDesc, slot);
		}
	}

}

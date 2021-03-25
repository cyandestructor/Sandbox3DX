#include "jasspch.h"
#include "ShaderReflection.h"

#include <d3dcompiler.h>
#pragma comment(lib, "dxguid.lib")

namespace Jass {

	void ShaderReflection::Reflect(const void* shaderData, size_t dataSize)
	{
		D3DReflect(shaderData, dataSize, IID_ID3D11ShaderReflection, (void**)&m_reflection);
	}

	ShaderDescription ShaderReflection::GetShaderDescription() const
	{
		JASS_CORE_ASSERT(m_reflection != nullptr, "The reflection is nullptr");

		return ShaderDescription(m_reflection);
	}

}

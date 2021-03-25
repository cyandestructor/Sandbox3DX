#ifndef SHADER_DESCRIPTION_H
#define SHADER_DESCRIPTION_H

#include "ConstantBufferDescription.h"

namespace Jass {

	class ShaderDescription
	{
	public:
		ShaderDescription(ID3D11ShaderReflection* reflection);

		const std::vector<ConstantBufferDescription>& GetConstantBuffers() const { return m_constantBuffers; }

	private:
		ID3D11ShaderReflection* m_reflection = nullptr;
		D3D11_SHADER_DESC m_description = {};

		std::vector<ConstantBufferDescription> m_constantBuffers;

	};

}

#endif // !SHADER_DESCRIPTION_H

#ifndef CONSTANT_BUFFER_DESCRIPTION_H
#define CONSTANT_BUFFER_DESCRIPTION_H

#include "VariableDescription.h"

namespace Jass {

	class ConstantBufferDescription
	{
	public:
		ConstantBufferDescription(ID3D11ShaderReflectionConstantBuffer* bufferReflection);

		std::string GetName() const { return m_description.Name; }

		const std::vector<VariableDescription>& GetVariables() const { return m_variables; }

	private:
		ID3D11ShaderReflectionConstantBuffer* m_bufferReflection = nullptr;
		D3D11_SHADER_BUFFER_DESC m_description = {};

		std::vector<VariableDescription> m_variables;
	};

}

#endif // !CONSTANT_BUFFER_DESCRIPTION_H

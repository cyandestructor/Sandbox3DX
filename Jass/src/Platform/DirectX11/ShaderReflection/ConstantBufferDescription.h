#ifndef CONSTANT_BUFFER_DESCRIPTION_H
#define CONSTANT_BUFFER_DESCRIPTION_H

#include "VariableDescription.h"

namespace Jass {

	class ConstantBufferDescription
	{
	public:
		ConstantBufferDescription(ID3D11ShaderReflectionConstantBuffer* bufferReflection,
			const D3D11_SHADER_BUFFER_DESC& description,
			unsigned int slot = 0);
		ConstantBufferDescription(ID3D11ShaderReflectionConstantBuffer* bufferReflection, unsigned int slot = 0);

		std::string GetName() const { return m_description.Name; }
		unsigned int GetSlot() const { return m_slot; }

		const std::vector<VariableDescription>& GetVariables() const { return m_variables; }

	private:
		ID3D11ShaderReflectionConstantBuffer* m_bufferReflection = nullptr;
		D3D11_SHADER_BUFFER_DESC m_description = {};

		unsigned int m_slot = 0u;

		std::vector<VariableDescription> m_variables;

		void GetVariables();
	};

}

#endif // !CONSTANT_BUFFER_DESCRIPTION_H

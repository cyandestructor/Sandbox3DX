#ifndef SHADER_REFLECTION_H
#define SHADER_REFLECTION_H

#include "ShaderDescription.h"

namespace Jass {

	class ShaderReflection
	{
	public:
		ShaderReflection() = default;
		ShaderReflection(const void* shaderData, size_t dataSize) { Reflect(shaderData, dataSize); }
		
		void Reflect(const void* shaderData, size_t dataSize);

		ShaderDescription GetShaderDescription() const;

	private:
		ID3D11ShaderReflection* m_reflection = nullptr;

	};

}

#endif // !SHADER_REFLECTION_H

#ifndef DIRECTX11_SHADER_H
#define DIRECTX11_SHADER_H

#include "Jass/Renderer/Shaders/Shader.h"
#include "DirectX11Graphics.h"

namespace Jass {

	class JASS_API DirectX11Shader : public Shader {

	public:
		DirectX11Shader(const std::string& filepath);
		DirectX11Shader(const std::string& name, const std::string& filepath);
		DirectX11Shader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);
		virtual ~DirectX11Shader() = default;

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* value, unsigned int count) override;
		virtual void SetFloat(const std::string& name, float value) override;
		virtual void SetFloat3(const std::string& name, const JVec3& vector) override;
		virtual void SetFloat4(const std::string& name, const JVec4& vector) override;
		virtual void SetMat4(const std::string& name, const JMat4& vector) override;

		virtual const std::string& GetName() const override { return m_name; }

	private:
		ComPtr<ID3D11VertexShader> m_vertexShader;
		ComPtr<ID3D11PixelShader> m_pixelShader;
		std::string m_name;

		void CompileFromFile(const std::string& filepath);
		void Compile(const std::string& vs, const std::string& ps);

		bool CheckCompilation(HRESULT result, const ComPtr<ID3DBlob>& errorsBlob);

	};

}

#endif // !DIRECTX11_SHADER_H

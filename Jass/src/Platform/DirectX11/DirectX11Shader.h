#ifndef DIRECTX11_SHADER_H
#define DIRECTX11_SHADER_H

#include "Jass/Renderer/Shaders/Shader.h"
#include "DirectX11ConstantBufferEx.h"

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

		std::vector<DirectX11ConstantBufferEx> m_vsConstantBuffers;
		std::vector<DirectX11ConstantBufferEx> m_psConstantBuffers;

		void CompileFromFile(const std::string& filepath);
		void Compile(const std::string& vs, const std::string& ps);

		bool CheckCompilation(HRESULT result, const ComPtr<ID3DBlob>& errorsBlob);

		template <typename T>
		void UpdateBufferElement(const std::string& name, const T& value);
	};

	template<typename T>
	void DirectX11Shader::UpdateBufferElement(const std::string& name, const T& value)
	{
		bool found = false;
		for (auto& constantBuffer : m_vsConstantBuffers)
		{
			auto& buffer = constantBuffer.GetBuffer();
			if (auto ref = buffer[name]; ref.Exists()) {
				ref = value;
				found = true;
			}
		}

		for (auto& constantBuffer : m_psConstantBuffers)
		{
			auto& buffer = constantBuffer.GetBuffer();
			if (auto ref = buffer[name]; ref.Exists()) {
				ref = value;
				found = true;
			}
		}

		if (!found) {
			JASS_CORE_WARN("{0} uniform was not found", name);
		}
	}
}

#endif // !DIRECTX11_SHADER_H

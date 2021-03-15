#include "jasspch.h"
#include "DirectX11Shader.h"

#include "Jass/Utility/JassUtilities.h"
#include <filesystem>

#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")

namespace Jass {

	static std::string FromBlob(const ComPtr<ID3DBlob>& blob)
	{
		size_t length = blob->GetBufferSize();

		char* buffer = new char[length + 1];
		strcpy_s(buffer, length, static_cast<char*>(blob->GetBufferPointer()));
		buffer[length] = 0;
		std::string str = buffer;
		delete[] buffer;

		return str;
	}

	DirectX11Shader::DirectX11Shader(const std::string& filepath)
	{
		CompileFromFile(filepath);

		//Get the name from the filepath
		std::filesystem::path shaderPath(filepath);
		JASS_CORE_ASSERT(shaderPath.has_stem(), "The filepath is not valid");
		m_name = shaderPath.stem().string();
	}

	DirectX11Shader::DirectX11Shader(const std::string& name, const std::string& filepath)
		:m_name(name)
	{
		CompileFromFile(filepath);
	}

	DirectX11Shader::DirectX11Shader(const std::string& name, const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc)
		:m_name(name)
	{
		Compile(vertexShaderSrc, fragmentShaderSrc);
	}

	void DirectX11Shader::Bind()
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();

		deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
		deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
	}

	void DirectX11Shader::Unbind()
	{
	}

	void DirectX11Shader::SetInt(const std::string& name, int value)
	{
	}

	void DirectX11Shader::SetIntArray(const std::string& name, int* value, unsigned int count)
	{
	}

	void DirectX11Shader::SetFloat(const std::string& name, float value)
	{
	}

	void DirectX11Shader::SetFloat3(const std::string& name, const JVec3& vector)
	{
	}

	void DirectX11Shader::SetFloat4(const std::string& name, const JVec4& vector)
	{
	}

	void DirectX11Shader::SetMat4(const std::string& name, const JMat4& vector)
	{
	}

	void DirectX11Shader::CompileFromFile(const std::string& filepath)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();
		
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorsBlob;

		std::wstring wFilepath = Utilities::Strings::ToWstring(filepath);

		unsigned int flags1 = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
#ifdef JASS_DEBUG
		flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // JASS_DEBUG

		HRESULT result = 0;

		result = D3DCompileFromFile(
			wFilepath.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"vsMain",
			"vs_5_0",
			flags1,
			0u,
			&shaderBlob,
			&errorsBlob
		);

		if (CheckCompilation(result, errorsBlob)) {
			device->CreateVertexShader(
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				nullptr,
				&m_vertexShader
			);
		}

		result = D3DCompileFromFile(
			wFilepath.c_str(),
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"psMain",
			"ps_5_0",
			flags1,
			0u,
			&shaderBlob,
			&errorsBlob
		);

		if (CheckCompilation(result, errorsBlob)) {
			device->CreatePixelShader(
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				nullptr,
				&m_pixelShader
			);
		}
	}

	void DirectX11Shader::Compile(const std::string& vs, const std::string& ps)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorsBlob;

		unsigned int flags1 = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
#ifdef JASS_DEBUG
		flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // JASS_DEBUG

		HRESULT result = 0;

		result = D3DCompile(
			vs.c_str(),
			vs.length(),
			nullptr,
			nullptr,
			nullptr,
			"vsMain",
			"vs_5_0",
			flags1,
			0u,
			&shaderBlob,
			&errorsBlob
		);

		if (CheckCompilation(result, errorsBlob)) {
			device->CreateVertexShader(
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				nullptr,
				&m_vertexShader
			);
		}

		result = D3DCompile(
			ps.c_str(),
			ps.length(),
			nullptr,
			nullptr,
			nullptr,
			"psMain",
			"ps_5_0",
			flags1,
			0u,
			&shaderBlob,
			&errorsBlob
		);

		if (CheckCompilation(result, errorsBlob)) {
			device->CreatePixelShader(
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				nullptr,
				&m_pixelShader
			);
		}
	}

	bool DirectX11Shader::CheckCompilation(HRESULT result, const ComPtr<ID3DBlob>& errorsBlob)
	{
		if (FAILED(result)) {
			if (result == D3D11_ERROR_FILE_NOT_FOUND) {
				JASS_CORE_ASSERT(false, "The shader file was not found");
			}
			else {
				JASS_CORE_ASSERT(errorsBlob->GetBufferPointer() != nullptr, FromBlob(errorsBlob));
			}

			return false;
		}

		return true;
	}

}

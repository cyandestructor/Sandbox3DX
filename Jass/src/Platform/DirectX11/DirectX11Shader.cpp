#include "jasspch.h"
#include "DirectX11Shader.h"

#include "Jass/Utility/JassUtilities.h"
#include "ShaderReflection/ShaderReflection.h"
#include <filesystem>

#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")


namespace Jass {

	static Dcb::Type ToElementType(ShaderVariableType type)
	{
		Dcb::Type elementType = Dcb::Type::Empty;

		switch (type)
		{
		case ShaderVariableType::Float:
			elementType = Dcb::Type::Float;
			break;
		case ShaderVariableType::Float2:
			elementType = Dcb::Type::Float2;
			break;
		case ShaderVariableType::Float3:
			elementType = Dcb::Type::Float3;
			break;
		case ShaderVariableType::Float4:
			elementType = Dcb::Type::Float4;
			break;
		case ShaderVariableType::Matrix3:
			elementType = Dcb::Type::Matrix3;
			break;
		case ShaderVariableType::Matrix4:
			elementType = Dcb::Type::Matrix4;
			break;
		case ShaderVariableType::Struct:
			elementType = Dcb::Type::Struct;
			break;
		case ShaderVariableType::Bool:
			elementType = Dcb::Type::Boolean;
			break;
		case ShaderVariableType::Int:
			elementType = Dcb::Type::Integer;
			break;
		default:
			JASS_CORE_ASSERT(false, "Unsupported variable type");
			break;
		}

		return elementType;
	}

	static void AddLayoutElement(Dcb::LayoutElement& layout, const std::string& name, const TypeDescription& variableType)
	{
		auto type = variableType.GetType();
		
		if (variableType.IsArray()) {
			layout.Add(Dcb::Type::Array, name);
			layout[name].Set(ToElementType(type), variableType.GetElementsCount());

			if (type == ShaderVariableType::Struct) {
				Dcb::LayoutElement& arrayType = layout[name].ArrayType();
				for (const auto& member : variableType.GetMembers()) {
					AddLayoutElement(arrayType, member.GetMemberName(), member);
				}
			}
		}
		else if (type == ShaderVariableType::Struct) {
			layout.Add(Dcb::Type::Struct, name);
			Dcb::LayoutElement& structElement = layout[name];
			for (const auto& member : variableType.GetMembers()) {
				AddLayoutElement(structElement, member.GetMemberName(), member);
			}
		}
		else {
			layout.Add(ToElementType(type), name);
		}
	}

	static Dcb::RawLayout CreateConstantBufferLayout(const ConstantBufferDescription& bufferDescription)
	{
		Dcb::RawLayout layout;

		for (const auto& variable : bufferDescription.GetVariables())
		{
			AddLayoutElement(layout.GetRoot(), variable.GetName(), variable.GetTypeDescription());
		}

		return layout;
	}

	static void GetConstantBuffersFromShader(const ComPtr<ID3DBlob>& shaderData,
		std::vector<DirectX11ConstantBufferEx>& constantBuffers,
		Destination destinationShader)
	{
		ShaderReflection shaderReflection(shaderData->GetBufferPointer(), shaderData->GetBufferSize());
		auto shaderDescription = shaderReflection.GetShaderDescription();

		constantBuffers.reserve(shaderDescription.GetConstantBuffers().size());
		for (const auto& buffer : shaderDescription.GetConstantBuffers()) {
			constantBuffers.emplace_back(
				std::move(CreateConstantBufferLayout(buffer)),
				destinationShader,
				buffer.GetSlot());
		}
	}

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

		for (const auto& constantBuffer : m_vsConstantBuffers) {
			constantBuffer.Bind();
		}

		for (const auto& constantBuffer : m_psConstantBuffers) {
			constantBuffer.Bind();
		}

		deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0u);
		deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0u);
	}

	void DirectX11Shader::Unbind()
	{
	}

	void DirectX11Shader::SetInt(const std::string& name, int value)
	{
		UpdateBufferElement<int>(name, value);
	}

	void DirectX11Shader::SetIntArray(const std::string& name, int* value, unsigned int count)
	{
	}

	void DirectX11Shader::SetFloat(const std::string& name, float value)
	{
		UpdateBufferElement<float>(name, value);
	}

	void DirectX11Shader::SetFloat3(const std::string& name, const JVec3& vector)
	{
		UpdateBufferElement<Jass::JVec3>(name, vector);
	}

	void DirectX11Shader::SetFloat4(const std::string& name, const JVec4& vector)
	{
		UpdateBufferElement<Jass::JVec4>(name, vector);
	}

	void DirectX11Shader::SetMat4(const std::string& name, const JMat4& vector)
	{
		UpdateBufferElement<Jass::JMat4>(name, vector);
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

			GetConstantBuffersFromShader(shaderBlob, m_vsConstantBuffers, Destination::VertexShader);
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

			GetConstantBuffersFromShader(shaderBlob, m_psConstantBuffers, Destination::PixelShader);
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
			
			GetConstantBuffersFromShader(shaderBlob, m_vsConstantBuffers, Destination::VertexShader);
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
			
			GetConstantBuffersFromShader(shaderBlob, m_psConstantBuffers, Destination::PixelShader);
		}
	}

	bool DirectX11Shader::CheckCompilation(HRESULT result, const ComPtr<ID3DBlob>& errorsBlob)
	{
		if (FAILED(result)) {
			if (result == D3D11_ERROR_FILE_NOT_FOUND || result == ERROR_FILE_NOT_FOUND) {
				JASS_CORE_ASSERT(false, "The shader file was not found");
			}
			else {
				JASS_CORE_ASSERT(errorsBlob->GetBufferPointer() != nullptr, FromBlob(errorsBlob).c_str());
			}

			return false;
		}

		return true;
	}

}

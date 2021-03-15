#include "jasspch.h"
#include "DirectX11VertexBuffer.h"

#include <d3dcompiler.h>

namespace Jass {

	static D3D11_USAGE DX11DataUsage(DataUsage usage)
	{
		// Temporary
		return D3D11_USAGE_DEFAULT;
	}

	static DXGI_FORMAT ToDXGIFormat(const BufferElement& element)
	{
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

		switch (element.Type)
		{
		case ShaderDataType::Float:
			format = DXGI_FORMAT_R32_FLOAT;
			break;
		case ShaderDataType::Float2:
			format = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case ShaderDataType::Float3:
			format = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case ShaderDataType::Float4:
			format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case ShaderDataType::Int:
			format = DXGI_FORMAT_R32_SINT;
			break;
		case ShaderDataType::Int2:
			format = DXGI_FORMAT_R32G32_SINT;
			break;
		case ShaderDataType::Int3:
			format = DXGI_FORMAT_R32G32B32_SINT;
			break;
		case ShaderDataType::Int4:
			format = DXGI_FORMAT_R32G32B32A32_SINT;
			break;
		default:
			JASS_CORE_ASSERT(false, "Unsupported shader data type");
			break;
		}

		return format;
	}

	static const char* ToHLSLTypeString(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:
			return "float";
		case ShaderDataType::Float2:
			return "float2";
		case ShaderDataType::Float3:
			return "float3";
		case ShaderDataType::Float4:
			return "float4";
		case ShaderDataType::Int:
			return "int";
		case ShaderDataType::Int2:
			return "int2";
		case ShaderDataType::Int3:
			return "int3";
		case ShaderDataType::Int4:
			return "int4";
		case ShaderDataType::Mat3:
			return "float3x3";
		case ShaderDataType::Mat4:
			return "float4x4";
		case ShaderDataType::Bool:
			return "bool";
		}

		JASS_CORE_ASSERT(false, "Unknown data type");
		return "";
	}

	DirectX11VertexBuffer::DirectX11VertexBuffer(const VertexBufferConfig& config)
	{
		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();

		D3D11_BUFFER_DESC bDesc = {};
		bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bDesc.Usage = DX11DataUsage(config.DataUsage);
		bDesc.CPUAccessFlags = 0u;
		bDesc.MiscFlags = 0u;
		bDesc.ByteWidth = config.Size;

		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = config.Data;

		device->CreateBuffer(&bDesc, &sd, &m_vertexBuffer);
	}

	DirectX11VertexBuffer::DirectX11VertexBuffer(unsigned int size)
	{
		JASS_CORE_ASSERT(false, "Not yet implemented");
	}

	void DirectX11VertexBuffer::Bind() const
	{
		auto& graphics = DirectX11Graphics::Get();
		auto deviceContext = graphics.GetDeviceContext();
		
		const unsigned int stride = m_layout.GetStride();
		const unsigned int offset = 0u;

		deviceContext->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->IASetInputLayout(m_inputLayout.Get());
	}

	void DirectX11VertexBuffer::Unbind() const
	{
	}

	void DirectX11VertexBuffer::SetLayout(const BufferLayout& layout)
	{
		m_layout = layout;

		auto& graphics = DirectX11Graphics::Get();
		auto device = graphics.GetDevice();

		std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
		elements.reserve(layout.GetElements().size());

		for (const auto& layoutElement : layout)
		{
			AddLayoutElement(layoutElement, elements);
		}

		// Create a dummy shader needed to check the input layout semantic names
		auto dummyShader = CreateDummyShader(layout);

		// Create the input layout object
		device->CreateInputLayout(
			&elements[0],
			(unsigned int)elements.size(),
			dummyShader->GetBufferPointer(),
			dummyShader->GetBufferSize(),
			&m_inputLayout
		);
	}

	void DirectX11VertexBuffer::SetData(const void* data, unsigned int size)
	{
		JASS_CORE_ASSERT(false, "Not yet implemented");
	}

	void DirectX11VertexBuffer::AddLayoutElement(const BufferElement& layoutElement, std::vector<D3D11_INPUT_ELEMENT_DESC>& elements)
	{
		ShaderDataType type = layoutElement.Type;

		if (type == ShaderDataType::Mat3) {
			for (unsigned int i = 0; i < 3; i++) {
				D3D11_INPUT_ELEMENT_DESC element = {};
				element.SemanticName = layoutElement.Name.c_str();
				element.SemanticIndex = i;
				element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
				element.InputSlot = 0u;
				// Add an offset of 12 bytes (3 floats) per component
				element.AlignedByteOffset = layoutElement.Offset + (i * 12u);
				element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				element.InstanceDataStepRate = 0u;

				elements.push_back(element);
			}
		}
		else if (type == ShaderDataType::Mat4) {
			for (unsigned int i = 0; i < 4; i++) {
				D3D11_INPUT_ELEMENT_DESC element = {};
				element.SemanticName = layoutElement.Name.c_str();
				element.SemanticIndex = i;
				element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				element.InputSlot = 0u;
				// Add an offset of 16 bytes (4 floats) per component
				element.AlignedByteOffset = layoutElement.Offset + (i * 16u);
				element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				element.InstanceDataStepRate = 0u;

				elements.push_back(element);
			}
		}
		else {
			D3D11_INPUT_ELEMENT_DESC element = {};
			element.SemanticName = layoutElement.Name.c_str();
			element.SemanticIndex = 0u;
			element.Format = ToDXGIFormat(layoutElement);
			element.InputSlot = 0u;
			element.AlignedByteOffset = layoutElement.Offset;
			element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			element.InstanceDataStepRate = 0u;

			elements.push_back(element);
		}
	}

	ComPtr<ID3DBlob> DirectX11VertexBuffer::CreateDummyShader(const BufferLayout& layout)
	{
		// Create a dummy shader with the layout semantics
		
		// Write the input layout with the proper semantic names
		std::stringstream ss;
		ss << "struct VS_INPUT {";
		int i = 0;
		for (const auto& element : layout) {
			ss << ToHLSLTypeString(element.Type) << " element" << i << " : " << element.Name << ";";
			i++;
		}
		ss << "};";

		// Write a dummy entry point
		ss << "float4 main(VS_INPUT input) : SV_Position { return float4(0.0f, 0.0f, 0.0f, 1.0f); }";

		// Compile dummy shader
		ComPtr<ID3DBlob> shaderBlob;

		HRESULT result = D3DCompile(
			ss.str().c_str(),
			ss.str().length(),
			nullptr,
			nullptr,
			nullptr,
			"main",
			"vs_5_0",
			0u,
			0u,
			&shaderBlob,
			nullptr
		);

		JASS_CORE_ASSERT(SUCCEEDED(result), "Dummy shader compilation failed");

		return shaderBlob;
	}

}

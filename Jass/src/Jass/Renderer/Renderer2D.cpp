#include "jasspch.h"
#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Shaders/ShaderLibrary.h"
#include "Shaders/Shader.h"

namespace Jass {

	struct Renderer2DStorage {
		// TODO: Change Ref with Scope
		Ref<Shader> ColorTextureShader;
		Ref<Texture2D> WhiteTexture;
		Ref<VertexArray> VertexArray;
	};

	static Renderer2DStorage* s_storage;

	void Renderer2D::Init()
	{
		JASS_PROFILE_FUNCTION();

		s_storage = new Renderer2DStorage;

		s_storage->ColorTextureShader = Shader::Create("assets/shaders/ColorTexture.glsl");
		s_storage->ColorTextureShader->Bind();
		s_storage->ColorTextureShader->SetInt("u_texture", 0);

		// Create a default white texture
		s_storage->WhiteTexture = Texture2D::Create(1, 1);
		uint32_t texData = 0xffffffff;
		s_storage->WhiteTexture->SetData(&texData, sizeof(uint32_t));
		// ------------------------------

		float positions[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,	// 0
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	// 1
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f,	// 2
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f	// 3
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		s_storage->VertexArray = Jass::VertexArray::Create();

		auto vertexBuffer = Jass::VertexBuffer::Create({ positions,sizeof(positions),Jass::DataUsage::StaticDraw });
		vertexBuffer->SetLayout({
			Jass::BufferElement(Jass::ShaderDataType::Float3,"in_position"),
			Jass::BufferElement(Jass::ShaderDataType::Float2,"in_texCoords")
			});

		s_storage->VertexArray->AddVertexBuffer(vertexBuffer);

		auto indexBuffer = Jass::IndexBuffer::Create({ indices,6,Jass::DataUsage::StaticDraw });

		s_storage->VertexArray->SetIndexBuffer(indexBuffer);
	}

	void Renderer2D::Shutdown()
	{
		delete s_storage;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		JASS_PROFILE_FUNCTION();

		s_storage->ColorTextureShader->Bind();
		s_storage->ColorTextureShader->SetMat4("u_viewProjection", camera.GetViewProjection());
	}

	void Renderer2D::EndScene()
	{
	}

	void Renderer2D::DrawQuad(const JVec2& position, const JVec2& scale, const JVec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, scale, color);
	}

	void Renderer2D::DrawQuad(const JVec3& position, const JVec2& scale, const JVec4& color)
	{
		JASS_PROFILE_FUNCTION();

		s_storage->ColorTextureShader->SetFloat4("u_color", color);

		JMat4 transformation = Translate(JMat4(1.0f), position);
		transformation = Scale(transformation, { scale.x, scale.y, 1.0f });
		s_storage->ColorTextureShader->SetMat4("u_transformation", transformation);

		s_storage->WhiteTexture->Bind();
		RenderCommand::DrawIndexed(s_storage->VertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const JVec2& position, float rotation, const JVec2& scale, const JVec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, rotation, scale, color);
	}

	void Renderer2D::DrawRotatedQuad(const JVec3& position, float rotation, const JVec2& scale, const JVec4& color)
	{
		JASS_PROFILE_FUNCTION();

		s_storage->ColorTextureShader->SetFloat4("u_color", color);

		JMat4 transformation = Translate(JMat4(1.0f), position);
		transformation = Rotate(transformation, rotation, { 0.0f,0.0f,1.0f });
		transformation = Scale(transformation, { scale.x, scale.y, 1.0f });
		s_storage->ColorTextureShader->SetMat4("u_transformation", transformation);

		s_storage->WhiteTexture->Bind();
		RenderCommand::DrawIndexed(s_storage->VertexArray);
	}

	void Renderer2D::DrawQuad(const JVec2& position, const JVec2& scale, const Ref<Texture2D>& texture, float tileFactor, const JVec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, scale, texture, tileFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const JVec3& position, const JVec2& scale, const Ref<Texture2D>& texture, float tileFactor, const JVec4& tintColor)
	{
		JASS_PROFILE_FUNCTION();

		s_storage->ColorTextureShader->SetFloat4("u_color", tintColor);
		s_storage->ColorTextureShader->SetFloat("u_tileFactor", tileFactor);

		JMat4 transformation = Translate(JMat4(1.0f), position);
		transformation = Scale(transformation, { scale.x, scale.y, 1.0f });
		s_storage->ColorTextureShader->SetMat4("u_transformation", transformation);

		texture->Bind();
		RenderCommand::DrawIndexed(s_storage->VertexArray);
	}

	void Renderer2D::DrawRotatedQuad(const JVec2& position, float rotation, const JVec2& scale, const Ref<Texture2D>& texture, float tileFactor, const JVec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, rotation, scale, texture, tileFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const JVec3& position, float rotation, const JVec2& scale, const Ref<Texture2D>& texture, float tileFactor, const JVec4& tintColor)
	{
		JASS_PROFILE_FUNCTION();

		s_storage->ColorTextureShader->SetFloat4("u_color", tintColor);
		s_storage->ColorTextureShader->SetFloat("u_tileFactor", tileFactor);
		
		JMat4 transformation = Translate(JMat4(1.0f), position);
		transformation = Rotate(transformation, rotation, { 0.0f,0.0f,1.0f });
		transformation = Scale(transformation, { scale.x, scale.y, 1.0f });
		s_storage->ColorTextureShader->SetMat4("u_transformation", transformation);

		texture->Bind();
		RenderCommand::DrawIndexed(s_storage->VertexArray);
	}

	void Renderer2D::DrawQuad(const QuadTransformation& transformation, const JVec4& color)
	{
		if (transformation.Rotation == 0.0f)
			DrawQuad(transformation.Position, transformation.Scale, color);
		else
			DrawRotatedQuad(transformation.Position, transformation.Rotation, transformation.Scale, color);
	}

	void Renderer2D::DrawQuad(const QuadTransformation& transformation, const TextureProps& textureProperties)
	{
		if (transformation.Rotation == 0.0f) {
			if (textureProperties.Texture != nullptr) {
				DrawQuad(transformation.Position, transformation.Scale,
					textureProperties.Texture, textureProperties.TileFactor, textureProperties.TintColor);
			}
			else {
				DrawQuad(transformation.Position, transformation.Scale,
					s_storage->WhiteTexture, textureProperties.TileFactor, textureProperties.TintColor);
			}
		}
		else {
			if (textureProperties.Texture != nullptr) {
				DrawRotatedQuad(transformation.Position, transformation.Rotation, transformation.Scale,
					textureProperties.Texture, textureProperties.TileFactor, textureProperties.TintColor);
			}
			else {
				DrawRotatedQuad(transformation.Position, transformation.Rotation, transformation.Scale,
					s_storage->WhiteTexture, textureProperties.TileFactor, textureProperties.TintColor);
			}
		}
	}

}

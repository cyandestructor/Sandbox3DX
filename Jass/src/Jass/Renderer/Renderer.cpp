#include "jasspch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Jass {
	
	// TEMPORARY
	std::unique_ptr<Renderer::SceneData> Renderer::sceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::OnWindowResize(unsigned int width, unsigned int height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const Camera& camera)
	{
		sceneData->ViewProjectionMatrix = camera.GetViewProjection();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, 
		const Ref<VertexArray>& vertexArray,
		const JMat4& transformation)
	{
		shader->Bind();
		shader->SetMat4("u_viewProjection", sceneData->ViewProjectionMatrix);
		shader->SetMat4("u_transformation", transformation);
		
		RenderCommand::DrawIndexed(vertexArray);
	}

	void Renderer::Submit(const Ref<Shader>& shader,
		const Ref<VertexArray>& vertexArray,
		RenderMode renderMode,
		const JMat4& transformation)
	{
		shader->Bind();
		shader->SetMat4("u_viewProjection", sceneData->ViewProjectionMatrix);
		shader->SetMat4("u_transformation", transformation);
		RenderCommand::DrawIndexed(vertexArray, 0, renderMode);
	}

}

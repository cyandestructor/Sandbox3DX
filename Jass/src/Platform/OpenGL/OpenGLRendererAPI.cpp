#include "jasspch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Jass {

	unsigned int ToGLEnum(RenderMode renderMode) {
		unsigned int mode = 0;
		
		switch (renderMode)
		{
		case RenderMode::Triangle:
			mode = GL_TRIANGLES;
			break;
		case RenderMode::TriangleStrip:
			mode = GL_TRIANGLE_STRIP;
			break;
		default:
			JASS_ASSERT(false, "Unknown render mode");
			break;
		}

		return mode;
	}

	unsigned int ToGLEnum(DepthFunc function) {
		unsigned int mode = 0;

		switch (function)
		{
		case DepthFunc::Never:
			mode = GL_NEVER;
			break;
		case DepthFunc::Less:
			mode = GL_LESS;
			break;
		case DepthFunc::Equal:
			mode = GL_EQUAL;
			break;
		case DepthFunc::LessEqual:
			mode = GL_LEQUAL;
			break;
		case DepthFunc::NotEqual:
			mode = GL_NOTEQUAL;
			break;
		case DepthFunc::Greater:
			mode = GL_GREATER;
			break;
		case DepthFunc::GreaterEqual:
			mode = GL_GEQUAL;
			break;
		case DepthFunc::Always:
			mode = GL_ALWAYS;
			break;
		default:
			JASS_CORE_ASSERT(false, "Unknown depth function");
			break;
		}

		return mode;
	}

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const JVec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::EnableClipDistance(bool enable, unsigned int index)
	{
		if (enable)
			glEnable(GL_CLIP_DISTANCE0 + index);
		else
			glDisable(GL_CLIP_DISTANCE0 + index);
	}

	void OpenGLRendererAPI::EnableDepthMask(bool enable)
	{
		glDepthMask(enable ? GL_TRUE : GL_FALSE);
	}

	void OpenGLRendererAPI::SetDepthFunction(DepthFunc function)
	{
		glDepthFunc(ToGLEnum(function));
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount, RenderMode renderMode)
	{
		JASS_PROFILE_FUNCTION();

		unsigned int count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();

		vertexArray->Bind();
		glDrawElements(ToGLEnum(renderMode), count, GL_UNSIGNED_INT, nullptr);
	}

}
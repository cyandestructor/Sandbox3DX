#ifndef RENDERER_API_H_JASS
#define RENDERER_API_H_JASS

#include "Jass/JMath/JMath.h"
#include "VertexArray.h"

namespace Jass {

	enum class RenderMode
	{
		Triangle, TriangleStrip
	};

	enum class DepthFunc
	{
		Never, Less, Equal, LessEqual, NotEqual, Greater, GreaterEqual, Always
	};

	enum class CullMode
	{
		None, Front, Back
	};

	class JASS_API RendererAPI {

	public:
		enum class API
		{
			None = 0, OpenGL, DirectX11
		};

		virtual void Init() = 0;

		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) = 0;

		virtual void SetClearColor(const JVec4& color) = 0;
		virtual void Clear() = 0;

		virtual void SetCullMode(CullMode mode) = 0;

		virtual void EnableClipDistance(bool enable, unsigned int index = 0) = 0;

		virtual void EnableDepthMask(bool enable) = 0;
		virtual void SetDepthFunction(DepthFunc function = DepthFunc::Less) = 0;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0, RenderMode renderMode = RenderMode::Triangle) = 0;

		static API GetAPI();

	private:
		static API s_API;

	};

}

#endif // !RENDERER_API_H_JASS

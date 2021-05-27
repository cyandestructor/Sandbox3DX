#ifndef DIRECTX11_RENDERER_API_H
#define DIRECTX11_RENDERER_API_H

#include "Jass/Renderer/RendererAPI.h"

namespace Jass {

	class JASS_API DirectX11RendererAPI : public RendererAPI {

	public:
		virtual void Init() override;

		virtual void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

		virtual void SetClearColor(const JVec4& color) override;
		virtual void Clear() override;

		virtual void SetCullMode(CullMode mode) override;

		virtual void EnableClipDistance(bool enable, unsigned int index = 0) override;

		virtual void EnableDepthMask(bool enable) override;
		virtual void SetDepthFunction(DepthFunc function = DepthFunc::Less) override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount = 0, RenderMode renderMode = RenderMode::Triangle) override;

	};

}

#endif // !DIRECTX11_RENDERER_API_H

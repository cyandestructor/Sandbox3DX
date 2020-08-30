#ifndef RENDERER2D_H_JASS
#define RENDERER2D_H_JASS

#include "Jass/Camera/OrthographicCamera.h"
#include "Jass/Renderer/Textures/Texture2D.h"

namespace Jass {

	struct JASS_API QuadTransformation {
		JVec3 Position = JVec3(0.0f);
		JVec2 Size = JVec2(1.0f);
		float Rotation = 0.0f;
	};

	struct JASS_API TextureProps {
		Ref<Texture2D> Texture;
		float TileFactor = 1.0f;
		JVec4 TintColor = JVec4(1.0f);
	};

	class JASS_API Renderer2D {

	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Flush();

		static void DrawQuad(const JVec2& position, const JVec2& size, const JVec4& color);
		static void DrawQuad(const JVec3& position, const JVec2& size, const JVec4& color);
		static void DrawRotatedQuad(const JVec2& position, float rotation, const JVec2& size, const JVec4& color);
		static void DrawRotatedQuad(const JVec3& position, float rotation, const JVec2& size, const JVec4& color);

		static void DrawQuad(const JVec2& position, const JVec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const JVec4& tintColor = JVec4(1.0f));
		static void DrawQuad(const JVec3& position, const JVec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const JVec4& tintColor = JVec4(1.0f));
		static void DrawRotatedQuad(const JVec2& position, float rotation, const JVec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const JVec4& tintColor = JVec4(1.0f));
		static void DrawRotatedQuad(const JVec3& position, float rotation, const JVec2& size, const Ref<Texture2D>& texture, float tileFactor = 1.0f, const JVec4& tintColor = JVec4(1.0f));

		static void DrawQuad(const QuadTransformation& transformation, const JVec4& color);
		static void DrawQuad(const QuadTransformation& transformation, const TextureProps& textureProperties);

	private:
		static void AddQuad(const JVec3& position, const JVec2& size, const JVec4& color, unsigned int texIndex, float tileFactor);
		static void AddRotatedQuad(const JVec3& position, const JVec2& size, float rotation, const JVec4& color, unsigned int texIndex, float tileFactor);
		static unsigned int SetTextureIndex(const Ref<Texture2D>& texture);

	};

}

#endif // !RENDERER2D_H_JASS

#ifndef JASS_OPENGLCONTEXT_H
#define JASS_OPENGLCONTEXT_H

#include "Jass/Renderer/IRenderContext.h"

struct GLFWwindow;

namespace Jass {

	class JASS_API OpenGLContext : public IRenderContext {

	public:
		OpenGLContext(GLFWwindow* windowHandler);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetVSync(bool enable) override;
		virtual bool IsVSync() const override { return m_isVSync; }

	private:
		GLFWwindow* m_windowHandler;
		bool m_isVSync = true;

	};

}

#endif // !JASS_OPENGLCONTEXT_H

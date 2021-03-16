#include "jasspch.h"
#include "OpenGLContext.h"

#ifdef JASS_DEBUG
#include "Jass/Debug/RendererDebugLog.h"
#endif // JASS_DEBUG

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Jass {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandler):
		m_windowHandler(windowHandler)
	{
		JASS_CORE_ASSERT(windowHandler, "The window handler is null");
	}

	void OpenGLContext::Init()
	{
		JASS_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_windowHandler);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		JASS_CORE_ASSERT(status, "Could not initialize Glad");

		JASS_CORE_INFO("OpenGL Info:");
		JASS_CORE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		JASS_CORE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		JASS_CORE_INFO("Version: {0}", glGetString(GL_VERSION));

#ifdef JASS_DEBUG
		RendererDebugLog::Init();
#endif // JASS_DEBUG

	}

	void OpenGLContext::SwapBuffers()
	{
		JASS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandler);
	}

	void OpenGLContext::SetVSync(bool enable)
	{
		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_isVSync = enable;
	}

}

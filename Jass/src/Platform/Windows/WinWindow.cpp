#include "jasspch.h"
#include "WinWindow.h"
#include "Jass/Events/Events.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

//#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/DirectX11/DirectX11Context.h"

namespace Jass {

	static void GLFWErrorCallback(int error, const char* description)
	{
		JASS_CORE_ERR("GLFW Error {0}: {1}", error, description);
	}

	IWindow* IWindow::Create(const WindowProps& properties)
	{
		return new WinWindow(properties);
	}

	bool WinWindow::s_glfwInitialized = false;
	std::vector<GLFWwindow*> WinWindow::s_windows;

	void WinWindow::OnUpdate()
	{
		glfwPollEvents();
		m_context->SwapBuffers();
	}

	inline void WinWindow::SetVSync(bool enable)
	{
		m_context->SetVSync(enable);

		m_windowData.VSync = enable;
	}

	void WinWindow::Init(const WindowProps& properties)
	{
		JASS_PROFILE_FUNCTION();

		m_windowData.Title = properties.Title;
		m_windowData.Width = properties.Width;
		m_windowData.Height = properties.Height;

		JASS_CORE_INFO("Creating window. Title: {0}, {1} X {2} px",
			m_windowData.Title, m_windowData.Width, m_windowData.Height);

		if (!s_glfwInitialized) {

			JASS_PROFILE_SCOPE("glfw Init");

			int success = glfwInit();
			JASS_CORE_ASSERT(success, "Could not initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_glfwInitialized = true;

		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		{
			JASS_PROFILE_SCOPE("glfw CreateWindow");
			m_window = glfwCreateWindow((int)m_windowData.Width, (int)m_windowData.Height,
				m_windowData.Title.c_str(), nullptr, nullptr);
		}

		if (!m_window) {
			JASS_CORE_ERR("Failed to create the window with title: {0}", m_windowData.Title);
		}

		//Create the context
		m_context = std::make_unique<DirectX11Context>(glfwGetWin32Window(m_window));
		m_context->Init();
		glfwSetWindowUserPointer(m_window, &m_windowData);
		SetVSync(true);

		// Set the GLFW callbacks
		SetWindowEventCallbacks();

		// s_windows are the total glfw windows instantiated
		s_windows.push_back(m_window);
	}

	void WinWindow::Shutdown()
	{
		// Remove the window from the instantiated glfw windows, then destroy it
		s_windows.erase(std::remove(s_windows.begin(), s_windows.end(), m_window), s_windows.end());
		glfwDestroyWindow(m_window);
	}

	void WinWindow::SetWindowEventCallbacks()
	{
		SetWindowSizeCallback();
		SetWindowCloseCallback();
		SetWindowKeyCallback();
		SetWindowCharCallback();
		SetWindowMouseButtonCallback();
		SetWindowMouseScrollCallback();
		SetWindowMouseMoveCallback();
		SetWindowJoystickConnectionCallback();
	}

	void WinWindow::SetWindowSizeCallback()
	{
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent e(width, height);
			data.EventCallback(e);
			});

	}

	void WinWindow::SetWindowCloseCallback()
	{
		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e;
			data.EventCallback(e);
			});
	}

	void WinWindow::SetWindowKeyCallback()
	{
		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					data.EventCallback(e);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1);
					data.EventCallback(e);
					break;
				}
			}
			});
	}

	void WinWindow::SetWindowCharCallback()
	{
		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent e(codepoint);

			data.EventCallback(e);
			});
	}

	void WinWindow::SetWindowMouseButtonCallback()
	{
		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallback(e);
					break;
				}
			}

			});
	}

	void WinWindow::SetWindowMouseMoveCallback()
	{
		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)x, (float)y);
			data.EventCallback(e);
			});
	}

	void WinWindow::SetWindowMouseScrollCallback()
	{
		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xOffset, (float)yOffset);
			data.EventCallback(e);

			});
	}

	void WinWindow::SetWindowJoystickConnectionCallback()
	{
		glfwSetJoystickCallback([](int joystickID, int isConnected) {
			Joystick joystick = Joystick(joystickID - GLFW_JOYSTICK_1);
			bool connection = isConnected == GLFW_CONNECTED;

			JoystickConnectionEvent e(joystick, connection);
			for (auto window : s_windows)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(e);
			}

			});
	}

}
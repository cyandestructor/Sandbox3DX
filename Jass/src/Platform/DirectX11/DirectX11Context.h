#ifndef DIRECTX11CONTEXT_H
#define DIRECTX11CONTEXT_H

#include "Jass/Renderer/IRenderContext.h"

namespace Jass {

	class JASS_API DirectX11Context : public IRenderContext {

	public:
		DirectX11Context(HWND windowHandler);

		virtual void Init() override;
		virtual void SwapBuffers() override;
		virtual void SetVSync(bool enable) override;
		virtual bool IsVSync() const override;

	private:
		HWND m_windowHandler;

	};

}

#endif // !DIRECTX11CONTEXT_H

#ifndef JASS_IRENDERCONTEXT_H
#define JASS_IRENDERCONTEXT_H

#include "Jass/Core/Core.h"

namespace Jass {

	class JASS_API IRenderContext {

	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
		virtual void SetVSync(bool enable) = 0;
		virtual bool IsVSync() const = 0;

	};

}

#endif // !JASS_IRENDERCONTEXT_H

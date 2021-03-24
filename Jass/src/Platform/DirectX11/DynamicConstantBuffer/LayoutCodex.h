#ifndef DCB_LAYOUT_CODEX_H
#define DCB_LAYOUT_CODEX_H

#include "RawLayout.h"
#include "FixedLayout.h"

namespace Dcb {

	class LayoutCodex
	{
	public:
		static FixedLayout Resolve(RawLayout&& layout);

	private:
		std::unordered_map<std::string, std::shared_ptr<LayoutElement>> m_map;

		static LayoutCodex& Get();

	};

}

#endif // !DCB_LAYOUT_CODEX_H

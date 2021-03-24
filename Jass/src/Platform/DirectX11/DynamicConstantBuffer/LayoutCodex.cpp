#include "jasspch.h"
#include "LayoutCodex.h"

namespace Dcb {

	FixedLayout LayoutCodex::Resolve(RawLayout&& layout)
	{
		auto signature = layout.GetSignature();
		auto& map = Get().m_map;

		const auto& i = map.find(signature);
		if (i != map.end())
		{
			layout.ClearRoot();
			return { i->second };
		}

		auto result = map.insert({ std::move(signature), layout.DeliverRoot() });
		return { result.first->second };
	}

	LayoutCodex& LayoutCodex::Get()
	{
		static LayoutCodex codex;
		return codex;
	}

}

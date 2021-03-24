#include "jasspch.h"
#include "FixedLayout.h"

namespace Dcb
{
	const LayoutElement& FixedLayout::operator[](const std::string& key)
	{
		return (*m_root)[key];
	}

	std::shared_ptr<LayoutElement> FixedLayout::ShareRoot() const
	{
		return m_root;
	}

	FixedLayout::FixedLayout(std::shared_ptr<LayoutElement> root)
		:Layout(std::move(root))
	{
	}

	std::shared_ptr<LayoutElement> FixedLayout::RenounceRoot()
	{
		return std::move(m_root);
	}
}

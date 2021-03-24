#include "jasspch.h"
#include "Layout.h"

namespace Dcb {

	size_t Layout::GetSizeInBytes() const
	{
		return m_root->GetSizeInBytes();
	}

	std::string Layout::GetSignature() const
	{
		return m_root->GetSignature();
	}

	Layout::Layout(std::shared_ptr<LayoutElement> root)
		:m_root(std::move(root))
	{
	}

}

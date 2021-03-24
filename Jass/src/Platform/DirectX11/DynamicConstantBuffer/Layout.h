#ifndef DCB_LAYOUT_H
#define DCB_LAYOUT_H

#include "LayoutElement.h"

namespace Dcb {

	class Layout
	{
		friend class LayoutCodex;
	public:
		size_t GetSizeInBytes() const;
		std::string GetSignature() const;

	protected:
		Layout(std::shared_ptr<LayoutElement> root);
		std::shared_ptr<LayoutElement> m_root;
	};

}

#endif // !DCB_LAYOUT_H

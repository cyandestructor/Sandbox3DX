#ifndef DCB_FIXED_LAYOUT_H
#define DCB_FIXED_LAYOUT_H

#include "Layout.h"

namespace Dcb
{
	class FixedLayout : public Layout
	{
		friend class Buffer;
		friend class LayoutCodex;
	public:
		const LayoutElement& operator[](const std::string& key);

		std::shared_ptr<LayoutElement> ShareRoot() const;

	private:
		FixedLayout(std::shared_ptr<LayoutElement> root);

		std::shared_ptr<LayoutElement> RenounceRoot();
	};
}

#endif // !DCB_FIXED_LAYOUT_H

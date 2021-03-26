#ifndef DCB_RAW_LAYOUT_H
#define DCB_RAW_LAYOUT_H

#include "Layout.h"

namespace Dcb {

	class RawLayout : public Layout
	{
		friend class LayoutCodex;
	public:
		RawLayout();

		LayoutElement& GetRoot() { return *m_root; }

		LayoutElement& operator[](const std::string& key);

		LayoutElement& Add(Type type, const std::string& name);

	private:
		void ClearRoot();

		std::shared_ptr<LayoutElement> DeliverRoot();
	};

}

#endif // !DCB_RAW_LAYOUT_H

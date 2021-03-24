#include "jasspch.h"
#include "RawLayout.h"

namespace Dcb {

	RawLayout::RawLayout()
		:Layout(std::shared_ptr<LayoutElement>(new LayoutElement(Type::Struct)))
	{
	}

	LayoutElement& RawLayout::operator[](const std::string& key)
	{
		return (*m_root)[key];
	}

	LayoutElement& RawLayout::Add(Type type, const std::string& name)
	{
		return m_root->Add(type, name);
	}

	void RawLayout::ClearRoot()
	{
		*this = RawLayout();
	}

	std::shared_ptr<LayoutElement> RawLayout::DeliverRoot()
	{
		auto temp = std::move(m_root);
		temp->Finalize(0);
		*this = RawLayout();
		return std::move(temp);
	}

}

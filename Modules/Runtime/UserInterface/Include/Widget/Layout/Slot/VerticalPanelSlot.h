#pragma once

#include "VerticalPanelSlot.gen.h"
#include "PanelWidgetSlot.h"
#include "Widget/Layout/VerticalPanel.h"

namespace Ry
{
	struct VerticalPanelSlot : public PanelWidgetSlot
	{
		VerticalPanelSlot() :
		PanelWidgetSlot()
		{
		}

		VerticalPanelSlot(SharedPtr<Ry::Widget> Wid) :
			PanelWidgetSlot(Wid)
		{
		}

		VerticalPanelSlot& operator[](SharedPtr<Ry::Widget> Child)
		{
			this->Widget = Child;
			return *this;
		}

	};
}

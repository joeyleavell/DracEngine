#pragma once

#include "PanelWidgetSlot.h"
#include "HorizontalPanelSlot.gen.h"

namespace Ry
{
	class HorizontalPanelSlot : public PanelWidgetSlot
	{
	public:
		GeneratedBody()
		
		HorizontalPanelSlot() :
		PanelWidgetSlot()
		{
		}

		HorizontalPanelSlot(SharedPtr<Ry::Widget> Wid) :
		PanelWidgetSlot(Wid)
		{
		}
	} RefClass();
}

#pragma once

#include "Widget/Widget.h"
#include "Color.h"
#include "Drawable.h"
#include "Style.h"
#include "Widget/Layout/SlotWidget.h"
#include "BorderWidget.gen.h"

namespace Ry
{
	class USERINTERFACE_MODULE BorderWidget : public SlotWidget
	{
	public:

		GeneratedBody()

		RefField()
		Ry::String BoxStyleName;
		
		BorderWidget();
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void Draw() override;
		bool HasVisual();
		void OnHovered(const MouseEvent& MouseEv) override;
		void OnUnhovered(const MouseEvent& MouseEv) override;
		bool OnPressed(const MouseButtonEvent& MouseEv) override;
		bool OnReleased(const MouseButtonEvent& MouseEv) override;

	private:

		Ry::SharedPtr<BatchItemSet> ItemSet;

		/**
		 * The styling for various states of this box element.
		 */
		//BoxStyle BStyle;
		
	} RefClass();
}

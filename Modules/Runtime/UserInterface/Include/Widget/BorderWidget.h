#pragma once

#include "Widget/Widget.h"
#include "Color.h"
#include "Drawable.h"
#include "Style.h"
#include "Widget/Layout/SlotWidget.h"
#include "UserInterfaceGen.h"

namespace Ry
{
	class USERINTERFACE_MODULE BorderWidget : public SlotWidget
	{
	public:

		WidgetBeginArgs(BorderWidget)
			WidgetProp(float, FillX)
			WidgetProp(float, FillY)
			WidgetPropDefault(float, Padding, 0.0f)
			WidgetPropDefault(HAlign, HorAlign, HAlign::CENTER)
			WidgetPropDefault(VAlign, VertAlign, VAlign::CENTER)
			WidgetPropDefault(bool, HasStyle, true)
			WidgetProp(Ry::String, BoxStyleName)
			// WidgetProp(Texture*, DefaultImage)
			// WidgetProp(Color, DefaultImageTint)
			// WidgetProp(Texture*, HoveredImage)
			// WidgetProp(Color, HoveredImageTint)
			// WidgetProp(Texture*, PressedImage)
			// WidgetProp(Color, PressedImageTint)
			// WidgetProp(BoxDrawable, DefaultBox)
			// WidgetProp(BoxDrawable, HoveredBox)
			// WidgetProp(BoxDrawable, PressedBox)
		WidgetEndArgs()

		void Construct(Args& In);
		BorderWidget();
		BorderWidget& Box(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize);
		BorderWidget& DefaultImage(Texture* Image, Color ImageTint = WHITE);
		BorderWidget& HoveredImage(Texture* Image, Color ImageTint = WHITE);
		BorderWidget& PressedImage(Texture* Image, Color ImageTint = WHITE);
		BorderWidget& DefaultBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize);
		BorderWidget& HoveredBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize);
		BorderWidget& PressedBox(const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize);
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void Draw() override;
		bool HasVisual();
		void OnHovered(const MouseEvent& MouseEv) override;
		void OnUnhovered(const MouseEvent& MouseEv) override;
		bool OnPressed(const MouseButtonEvent& MouseEv) override;
		bool OnReleased(const MouseButtonEvent& MouseEv) override;

	private:

		Ry::String BoxStyleName;

		/**
		 * The styling for various states of this box element.
		 */
		//BoxStyle BStyle;
		
	};
}

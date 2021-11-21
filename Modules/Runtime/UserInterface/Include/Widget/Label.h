#pragma once

#include "Widget/Widget.h"
#include "RenderingPass.h"
#include "Font.h"
#include "Color.h"
#include "Style.h"
#include "Label.gen.h"

namespace Ry
{
	class USERINTERFACE_MODULE Label : public Widget
	{
	public:

		GeneratedBody()

		RefField()
		String Text;

		TextStyle Style;

		WidgetBeginArgs(Label)
			WidgetProp(Ry::String, Text)
			WidgetProp(BitmapFont*, Font)
			WidgetProp(Ry::Color, Color)
		WidgetEndArgs()

		Label();
		void Construct(Args& In);
		SizeType ComputeSize() const override;
		Label& SetText(const Ry::String& Text);
		Label& SetStyle(BitmapFont* Font, const Color& Color);
		Label& SetFont(BitmapFont* Font);
		const Ry::String& GetText() const;
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void Draw(StyleSet* TheStyle) override;

	private:

		void ComputeTextData();

		mutable SizeType CachedSize;
		mutable bool bTextSizeDirty;
		PrecomputedTextData ComputedTextData;				
		Ry::SharedPtr<BatchItemSet> ItemSet;
	} RefClass();
}

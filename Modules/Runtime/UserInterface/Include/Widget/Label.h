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
		Ry::String Text;

		RefField()
		Ry::String TextStyleName;

		WidgetBeginArgs(Label)
			WidgetProp(Ry::String, Text)
			WidgetProp(Ry::String, TextStyleName)
		WidgetEndArgs()

		Label();
		void Construct(Args& In);
		SizeType ComputeSize() const override;
		Label& SetText(const Ry::String& Text);
		Label& SetTextStyle(const Ry::String& StyleName);
		const Ry::String& GetText() const;
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void Draw() override;
		void SetStyle(const Ry::StyleSet* Style) override;

	private:

		void ComputeTextData();

		mutable SizeType CachedSize;
		mutable bool bTextSizeDirty;
		PrecomputedTextData ComputedTextData;				
		Ry::SharedPtr<BatchItemSet> ItemSet;
	} RefClass();
}

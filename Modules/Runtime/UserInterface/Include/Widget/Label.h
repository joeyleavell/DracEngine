#pragma once

#include "Widget/Widget.h"
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

		RefField()
		int32 MinimumWidth;

		Label();
		SizeType ComputeSize() const override;
		Label& SetText(const Ry::String& Text);
		Label& SetTextStyle(const Ry::String& StyleName);
		const Ry::String& GetText() const;
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void Draw() override;
		void SetVisibleFlag(bool bVisibility, bool bPropagate) override;

	private:

		void ComputeTextData();

		mutable SizeType CachedSize;
		mutable bool bTextSizeDirty;
		PrecomputedTextData ComputedTextData;				
		Ry::SharedPtr<BatchItemSet> ItemSet;
	} RefClass();
}

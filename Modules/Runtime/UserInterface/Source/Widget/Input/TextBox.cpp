#include "Widget/Input/TextBox.h"

namespace Ry
{

	TextBox::TextBox()
	{
		InnerTextField = MakeShared(new TextField);
		SetChild(InnerTextField);

		BoxStyleName = "TextBox";

		this->Padding(6.0f);
	}

	SharedPtr<TextField> TextBox::GetTextField() const
	{
		return InnerTextField;
	}

	Ry::String TextBox::GetText() const
	{
		return InnerTextField->GetText();
	}

	void TextBox::SetText(const Ry::String& Text)
	{
		InnerTextField->SetText(Text);
	}

}

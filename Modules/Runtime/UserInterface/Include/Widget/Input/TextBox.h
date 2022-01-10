#pragma once

#include "TextBox.gen.h"
#include "TextField.h"
#include "Widget/BorderWidget.h"

namespace Ry
{

	class USERINTERFACE_MODULE TextBox : public BorderWidget
	{
	public:

		TextBox();

		SharedPtr<TextField> GetTextField() const;

		Ry::String GetText() const;
		void SetText(const Ry::String& Text);

	private:

		SharedPtr<TextField> InnerTextField;

	} RefClass();

}

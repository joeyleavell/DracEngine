#pragma once

#include "Widget/Layout/SlotWidget.h"
#include "Widget/Layout/VerticalPanel.h"

namespace Ry
{
	class DetailsView : public SlotWidget
	{
	public:
		DetailsView();
		void SetObject(Ry::Object* DetailsObject);

	private:
		Ry::SharedPtr<Ry::VerticalPanel> DetailsPanel;

		void CreateWidgetForField(const Ry::Object* Object, const Ry::Field& Field);
		void CreateStringWidgetForField(const Ry::Object* Object, const Ry::Field& Field);

	};
}

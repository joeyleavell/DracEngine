#include "DetailsView.h"

#include "Widget/Label.h"
#include "Widget/Input/TextField.h"

namespace Ry
{

	DetailsView::DetailsView()
	{
		DetailsPanel = MakeShared(new Ry::VerticalPanel);
		SetChild(DetailsPanel);
	}

	void DetailsView::SetObject(Ry::Object* DetailsObject)
	{
		// Clear existing children
		DetailsPanel->ClearChildren();

		for(const Ry::Field& Field : DetailsObject->GetClass()->Fields)
		{
			CreateWidgetForField(DetailsObject, Field);
		}
	}

	void DetailsView::CreateWidgetForField(const Ry::Object* Object, const Ry::Field& Field)
	{
		if(Field.Type->Name == GetType<Ry::String>()->Name)
		{
			CreateStringWidgetForField(Object, Field);
		}
	}

	void DetailsView::CreateStringWidgetForField(const Ry::Object* Object, const Ry::Field& Field)
	{
		SharedPtr<Ry::HorizontalPanel> FieldRow = MakeShared(new Ry::HorizontalPanel);
		SharedPtr<Ry::Label> RowLabel = MakeShared(new Ry::Label);
		SharedPtr<Ry::TextField> StringField = MakeShared(new Ry::TextField);

		RowLabel->TextStyleName = "Normal";
		RowLabel->SetText(Field.Name);

		StringField->TextStyleName = "Normal";
		StringField->SetText(*Field.GetConstPtrToField<Ry::String>(Object)); // Set the initial text

		SharedPtr<PanelWidgetSlot> LabelSlot = FieldRow->AppendSlot(RowLabel);
		SharedPtr<PanelWidgetSlot> ValueSlot = FieldRow->AppendSlot(StringField);

		LabelSlot->SetPadding(5.0f);
		ValueSlot->SetPadding(5.0f);

		SharedPtr<PanelWidgetSlot> Row = DetailsPanel->AppendSlot(FieldRow);
		Row->SetPadding(10.0f);
	}



}

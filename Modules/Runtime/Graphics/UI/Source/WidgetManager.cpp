#include "WidgetManager.h"
#include "rapidxml.hpp"
#include "File/File.h"
#include "Widget/VerticalPanel.h"
#include "Widget/HorizontalPanel.h"
#include "Widget/ScrollPane.h"
#include "Widget/GridLayout.h"
#include "Widget/SlotWidget.h"
#include "Widget/BorderWidget.h"
#include "Widget/Button.h"
#include "Widget/Label.h"
#include "Widget/TextField.h"

using namespace rapidxml;

namespace Ry
{
	WidgetManager::WidgetManager()
	{
		
	}

	WidgetManager::~WidgetManager()
	{
		
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidget(Ry::AssetRef& Path)
	{
		Ry::String AsVirtual = Path.GetVirtual();
		if (CachedWidgets.Contains(AsVirtual))
		{
			return LoadWidget_Internal(CachedWidgets.Get(AsVirtual)->Root);
		}
		else
		{
			SharedPtr<CachedWidget> NewCache = MakeShared(new CachedWidget);
			NewCache->XmlContents = Ry::File::LoadFileAsString2(Path.GetAbsolute());

			xml_document<> AltDoc;
			bool bParseError = false;

			try
			{
				AltDoc.parse<0>(*NewCache->XmlContents);
			}
			catch(parse_error E)
			{
				// Failed to parse, don't cache
				Ry::Log->LogErrorf("Failed to load widget from XML file due to parse error: %s", E.what());
				return MakeShared<Ry::Widget>(nullptr);
			}

			// Get first root node of XML
			NewCache->Root = AltDoc.first_node();

			// Kick out if there are more than one root nodes, don't cache
			if(AltDoc.last_node() != AltDoc.first_node())
			{
				Ry::Log->LogErrorf("Document %s has multiple root nodes, must only have one", *Path.GetVirtual());
				return MakeShared<Ry::Widget>(nullptr);
			}

			CachedWidgets.Insert(AsVirtual, NewCache);

			return LoadWidget_Internal(NewCache->Root);
		}
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidget_Internal(rapidxml::xml_node<>* Node)
	{
		Widget* Result = nullptr;

		// TODO: load stuff

		return MakeShared(Result);
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetSingle(rapidxml::xml_node<>* Node)
	{
		Ry::String Name = Node->name();
		Ry::SharedPtr<Widget> Result;

		if (Name == "VerticalBox")
			Result = LoadWidgetVerticalBox(Node);
		if (Name == "HorizontalBox")
			Result = LoadWidgetHorizontalBox(Node);
		if (Name == "ScrollBox")
			Result = LoadWidgetScrollBox(Node);
		if (Name == "GridBox")
			Result = LoadWidgetGridBox(Node);
		if (Name == "Slot")
			Result = LoadWidgetSlot(Node);
		if (Name == "Border")
			Result = LoadWidgetBorder(Node);
		if (Name == "Button")
			Result = LoadWidgetButton(Node);
		if (Name == "Label")
			Result = LoadWidgetButton(Node);
		if (Name == "TextField")
			Result = LoadWidgetTextField(Node);

		// Id attribute
		if(xml_attribute<>* IdAttrib = Node->first_attribute("Id"))
		{
			Result->SetId(IdAttrib->value());
		}

		// Class attribute
		if (xml_attribute<>* ClassAttrib = Node->first_attribute("Class"))
		{
			Result->SetClass(ClassAttrib->value());
		}

		// Style attribute
		if (xml_attribute<>* StyleAttrib = Node->first_attribute("Style"))
		{
			Result->SetStyleName(StyleAttrib->value());
		}

		return Result;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetVerticalBox(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::VerticalLayout> Res = NewWidget(Ry::VerticalLayout);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetHorizontalBox(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::HorizontalLayout> Res = NewWidget(Ry::HorizontalLayout);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetScrollBox(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::ScrollPane> Res = NewWidget(Ry::ScrollPane);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetGridBox(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::GridLayout> Res = NewWidget(Ry::GridLayout);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetSlot(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::SlotWidget> Res = NewWidget(Ry::SlotWidget);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetBorder(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::BorderWidget> Res = NewWidget(Ry::BorderWidget);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetButton(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::Button> Res = NewWidget(Ry::Button);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetLabel(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::Label> Res = NewWidget(Ry::Label);

		return Res;
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetTextField(rapidxml::xml_node<>* Node)
	{
		Ry::SharedPtr<Ry::TextField> Res = NewWidget(Ry::TextField);

		return Res;
	}
	
}

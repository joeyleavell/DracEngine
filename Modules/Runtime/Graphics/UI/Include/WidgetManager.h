#pragma once

#include "Data/Map.h"
#include "Widget/Widget.h"
#include "WidgetManager.gen.h"

namespace rapidxml
{
	template<class Ch>
	class xml_node;
	
}

namespace Ry
{
	class AssetRef;

	class UI_MODULE WidgetManager
	{
	public:
		WidgetManager();
		~WidgetManager();

		SharedPtr<Ry::Widget> LoadWidget(Ry::AssetRef& Path);

	private:

		struct CachedWidget
		{
			Ry::String XmlContents;
			rapidxml::xml_node<char>* Root;
		};

		SharedPtr<Ry::Widget> LoadWidget_Internal(rapidxml::xml_node<char>* Node);

		SharedPtr<Ry::Widget> LoadWidgetSingle(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetVerticalBox(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetHorizontalBox(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetScrollBox(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetGridBox(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetSlot(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetBorder(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetButton(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetLabel(rapidxml::xml_node<char>* Node);
		SharedPtr<Ry::Widget> LoadWidgetTextField(rapidxml::xml_node<char>* Node);

		Ry::OAHashMap<Ry::String, SharedPtr<CachedWidget>> CachedWidgets;
	};
	
}

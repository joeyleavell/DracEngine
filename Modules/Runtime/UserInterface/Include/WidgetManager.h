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

	class USERINTERFACE_MODULE WidgetManager
	{
	public:
		WidgetManager();
		~WidgetManager();

		SharedPtr<Ry::Widget> LoadWidget(const Ry::AssetRef& Path);

	private:

		struct CachedWidget
		{
			Ry::String XmlContents;
			rapidxml::xml_node<char>* Root;
		};

		SharedPtr<Ry::Widget> LoadWidgetSingle(rapidxml::xml_node<char>* Node);

		Ry::OAHashMap<Ry::String, SharedPtr<CachedWidget>> CachedWidgets;
	};

	extern WidgetManager Manager;

	USERINTERFACE_MODULE SharedPtr<Ry::Widget> LoadWidget(Ry::AssetRef&& Path);
	USERINTERFACE_MODULE SharedPtr<Ry::Widget> LoadWidget(const Ry::AssetRef& Path);

}

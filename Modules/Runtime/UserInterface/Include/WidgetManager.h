#pragma once

#include "Data/Map.h"
#include "Widget/Widget.h"
#include "WidgetManager.gen.h"

namespace rapidxml
{
	template<class Ch>
	class xml_node;

	template<class Ch>
	class xml_document;
	
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
			rapidxml::xml_document<char>* Root;
		};

		SharedPtr<Ry::Widget> LoadWidgetSingle(rapidxml::xml_node<char>* Node);

		Ry::OAHashMap<Ry::String, SharedPtr<CachedWidget>> CachedWidgets;
	};

	extern USERINTERFACE_MODULE WidgetManager Manager;

	template<typename WidgetClass>
	EXPORT_ONLY SharedPtr<WidgetClass> LoadWidget(Ry::AssetRef&& Path)
	{
		return CastShared<WidgetClass>(Manager.LoadWidget(Path));
	}

	template<typename WidgetClass>
	EXPORT_ONLY SharedPtr<WidgetClass> LoadWidget(const Ry::AssetRef& Path)
	{
		return CastShared<WidgetClass>(Manager.LoadWidget(Path));
	}

}

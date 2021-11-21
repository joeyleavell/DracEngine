#include "WidgetManager.h"
#include "rapidxml.hpp"
#include "File/File.h"
#include "Widget/BorderWidget.h"
#include "Widget/Label.h"

using namespace rapidxml;

namespace Ry
{
	WidgetManager Manager;

	WidgetManager::WidgetManager()
	{
		
	}

	WidgetManager::~WidgetManager()
	{
		
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidget(const Ry::AssetRef& Path)
	{
		Ry::String AsVirtual = Path.GetVirtual();
		if (CachedWidgets.Contains(AsVirtual))
		{
			return LoadWidgetSingle(CachedWidgets.Get(AsVirtual)->Root);
		}
		else
		{
			SharedPtr<CachedWidget> NewCache = MakeShared(new CachedWidget);
			NewCache->XmlContents = Ry::File::LoadFileAsString2(Path.GetAbsolute());

			xml_document<> AltDoc;

			try
			{
				AltDoc.parse<0>(*NewCache->XmlContents);
			}
			catch(const parse_error& E)
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

			return LoadWidgetSingle(NewCache->Root);
		}
	}

	SharedPtr<Ry::Widget> WidgetManager::LoadWidgetSingle(rapidxml::xml_node<>* Node)
	{
		Ry::String Name = Node->name();

		Ry::SharedPtr<Widget> Result;

		
		if(const Ry::ReflectedClass* WidgetClass = GetReflectedClass(Name))
		{
			// Create new widget
			Result = MakeShared(WidgetClass->CreateInstance<Ry::Widget>());

			if(Result.IsValid())
			{
				// Set the attributes of the newly created widget
				xml_attribute<>* Attrib = Node->first_attribute();
				while (Attrib)
				{
					Ry::String AttribName = Attrib->name();

					if (const Ry::Field* Field = WidgetClass->FindFieldByName(AttribName))
					{
						// Assign string property
						if(Field->Type->Name == "Ry::String")
							(*Field->GetPtrToField<Ry::String>(Result.Get())) = Attrib->value();
					}
					else
					{
						Ry::Log->LogErrorf("Failed to find field with name %s in widget of class %s", *AttribName, *Name);
					}

					Attrib = Attrib->next_attribute();
				}

				// Load all children of this widget
				Ry::ArrayList<Ry::SharedPtr<Widget>> ChildrenWidgets;
				xml_node<>* ChildNode = Node->first_node();
				while(ChildNode)
				{
					Ry::SharedPtr<Widget> Result = LoadWidgetSingle(ChildNode);
					ChildrenWidgets.Add(Result);
					ChildNode = ChildNode->next_sibling();
				}

				if(Ry::SlotWidget* AsSlot = dynamic_cast<Ry::SlotWidget*>(Result.Get()))
				{
					// Assert that there is only 1 child widget
					CORE_ASSERTF(ChildrenWidgets.GetSize() == 1, "There must only be 1 child of a slot widget");

					AsSlot->SetChild(ChildrenWidgets[0]);
				}

				// if (Ry::PanelWidget* AsPanel = dynamic_cast<Ry::PanelWidget*>(Result.Get()))
				// {
				// 	AsPanel->
				// }
			}
			else
			{
				Ry::Log->LogErrorf("Failed to create widget of class %s", *Name);
			}
		}
		else
		{
			Ry::Log->LogErrorf("Failed to get widget class %s", *Name);
		}

		return Result;
	}

	SharedPtr<Ry::Widget> LoadWidget(Ry::AssetRef&& Path)
	{
		return Manager.LoadWidget(Path);
	}

	SharedPtr<Ry::Widget> LoadWidget(const Ry::AssetRef& Path)
	{
		return Manager.LoadWidget(Path);
	}
	
}

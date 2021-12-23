#include "WidgetManager.h"
#include "rapidxml.hpp"
#include "File/AssetRef.h"
#include "File/File.h"
#include "Widget/BorderWidget.h"
#include "Widget/Layout/PanelWidget.h"
#include "Core/Globals.h"

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
			return LoadWidgetSingle(CachedWidgets.Get(AsVirtual)->Root->first_node());
		}
		else
		{
			SharedPtr<CachedWidget> NewCache = MakeShared(new CachedWidget);
			NewCache->XmlContents = Ry::File::LoadFileAsString2(Path.GetAbsolute());

			xml_document<>* AltDoc = new xml_document<>;

			try
			{
				AltDoc->parse<0>(*NewCache->XmlContents);
			}
			catch(const parse_error& E)
			{
				// Failed to parse, don't cache
				Ry::Log->LogErrorf("Failed to load widget from XML file due to parse error: %s at %d", E.what(), E.where<char>());
				return MakeShared<Ry::Widget>(nullptr);
			}

			// Get first root node of XML
			NewCache->Root = AltDoc;

			// Kick out if there are more than one root nodes, don't cache
			if(AltDoc->last_node() != AltDoc->first_node())
			{
				Ry::Log->LogErrorf("Document %s has multiple root nodes, must only have one", *Path.GetVirtual());
				return MakeShared<Ry::Widget>(nullptr);
			}

			CachedWidgets.Insert(AsVirtual, NewCache);

			return LoadWidgetSingle(NewCache->Root->first_node());
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
						// Strings
						if(Field->Type->Name == GetType<Ry::String>()->Name)
							(*Field->GetMutablePtrToField<Ry::String>(Result.Get())) = Attrib->value();

						// Floats
						if (Field->Type->Name == GetType<float>()->Name)
							(*Field->GetMutablePtrToField<float>(Result.Get())) = Ry::ParseFloat(Attrib->value());

						// uint8s
						if (Field->Type->Name == GetType<uint8>()->Name)
							(*Field->GetMutablePtrToField<uint8>(Result.Get())) = (uint8) Ry::ParseInt(Attrib->value());

					}
					else
					{
						Ry::Log->LogErrorf("Failed to find field with name %s in widget of class %s", *AttribName, *Name);
					}

					Attrib = Attrib->next_attribute();
				}

				if (Ry::PanelWidget* AsPanel = dynamic_cast<Ry::PanelWidget*>(Result.Get()))
				{
					// If this is a panel widget, expect all of the children to be slot widgets

					xml_node<>* ChildNode = Node->first_node();
					while (ChildNode)
					{
						CORE_ASSERTF(Ry::String(ChildNode->name()) == "Slot", "Children elements of PanelWidget must be named Slot");

						// Count the number of children of the slot element

						// Assert that this child slot has only a single child
						CORE_ASSERTF((ChildNode->first_node() != nullptr) && (ChildNode->first_node()->next_sibling() == nullptr), "There must be only a single child of slot elements");

						if(Ry::SharedPtr<Widget> SlotChild = LoadWidgetSingle(ChildNode->first_node()))
						{
							SharedPtr<PanelWidgetSlot> Slot = AsPanel->AppendSlot(SlotChild);
							
							// Set elements of panel slot
							if (const Ry::ReflectedClass* SlotClass = Slot->GetClass())
							{
								xml_attribute<>* SlotAttrib = ChildNode->first_attribute();
								while (SlotAttrib)
								{
									Ry::String SlotAttribName = SlotAttrib->name();

									if (const Ry::Field* Field = SlotClass->FindFieldByName(SlotAttribName))
									{
										// Strings
										if (Field->Type->Name == GetType<Ry::String>()->Name)
											(*Field->GetMutablePtrToField<Ry::String>(Slot.Get())) = SlotAttrib->value();

										// Floats
										if (Field->Type->Name == GetType<float>()->Name)
											(*Field->GetMutablePtrToField<float>(Slot.Get())) = Ry::ParseFloat(SlotAttrib->value());

										// uint8s
										if (Field->Type->Name == GetType<uint8>()->Name)
											(*Field->GetMutablePtrToField<uint8>(Slot.Get())) = (uint8)Ry::ParseInt(SlotAttrib->value());

									}
									else
									{
										Ry::Log->LogErrorf("Failed to find field with name %s in slot of class %s", *SlotAttribName, *Slot->GetClass()->Name);
									}

									SlotAttrib = SlotAttrib->next_attribute();
								}
							}
							else
							{
								Ry::Log->LogErrorf("Failed to get slot class");
							}

						}
						else
						{
							Ry::Log->LogErrorf("Failed to load slot widget %s", ChildNode->first_node()->name());
						}
						
						ChildNode = ChildNode->next_sibling();
					}
				}
				else
				{
					// Since we're not a panel, children can be anything
					
					// Load all children of this widget
					Ry::ArrayList<Ry::SharedPtr<Widget>> ChildrenWidgets;
					xml_node<>* ChildNode = Node->first_node();
					while (ChildNode)
					{
						Ry::SharedPtr<Widget> Result = LoadWidgetSingle(ChildNode);
						ChildrenWidgets.Add(Result);
						ChildNode = ChildNode->next_sibling();
					}

					if (Ry::SlotWidget* AsSlot = dynamic_cast<Ry::SlotWidget*>(Result.Get()))
					{
						// Assert that there is only 1 child widget
						CORE_ASSERTF(ChildrenWidgets.GetSize() == 1 || ChildrenWidgets.GetSize() == 0, "There must either be 1 or 0 children of a slot widget");

						if(ChildrenWidgets.GetSize() == 1 && ChildrenWidgets[0].IsValid())
						{
							AsSlot->SetChild(ChildrenWidgets[0]);
						}
					}
				}

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
	
}

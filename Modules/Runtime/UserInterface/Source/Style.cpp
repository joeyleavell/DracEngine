#include "Style.h"

namespace Ry
{
	Map<Ry::String, const StyleSet*> RegisteredStyles;
	StyleSet EditorStyle;

	StyleSet::StyleSet()
	{
		
	}

	StyleSet::~StyleSet()
	{
		
	}

	void StyleSet::AddTextStyle(Ry::String Name, const TextStyle& TextStyle)
	{
		TextStyles.Insert(Name, TextStyle);
	}

	void StyleSet::AddBoxStyle(Ry::String Name, const BoxStyle& BoxStyle)
	{
		BoxStyles.Insert(Name, BoxStyle);
	}

	void StyleSet::RemoveTextStyle(Ry::String Name)
	{
		TextStyles.Remove(Name);
	}

	void StyleSet::RemoveBoxStyle(Ry::String Name)
	{
		BoxStyles.Remove(Name);
	}

	const TextStyle& StyleSet::GetTextStyle(Ry::String Name) const
	{
		return TextStyles.Get(Name);
	}

	const BoxStyle& StyleSet::GetBoxStyle(Ry::String Name) const
	{
		return BoxStyles.Get(Name);
	}

	void RegisterStyle(Ry::String Name, const StyleSet* Set)
	{
		RegisteredStyles.insert(Name, Set);
	}

	void UnregisterStyle(Ry::String Name)
	{
		RegisteredStyles.remove(Name);
	}

	const StyleSet* GetStyle(Ry::String Name)
	{
		return *RegisteredStyles.get(Name);
	}
	
}

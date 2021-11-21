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
		TextStyles.insert(Name, TextStyle);
	}

	void StyleSet::AddBoxStyle(Ry::String Name, const BoxStyle& BoxStyle)
	{
		BoxStyles.insert(Name, BoxStyle);
	}

	void StyleSet::RemoveTextStyle(Ry::String Name)
	{
		TextStyles.remove(Name);
	}

	void StyleSet::RemoveBoxStyle(Ry::String Name)
	{
		BoxStyles.remove(Name);
	}

	const TextStyle& StyleSet::GetTextStyle(Ry::String Name) const
	{
		return *TextStyles.get(Name);
	}

	const BoxStyle& StyleSet::GetBoxStyle(Ry::String Name) const
	{
		return *BoxStyles.get(Name);
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

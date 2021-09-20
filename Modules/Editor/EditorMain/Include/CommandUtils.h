#pragma once

#include "Core/Core.h"

namespace Ry
{

	bool HasOption(Ry::ArrayList<Ry::String>& Args, Ry::String Option)
	{
		for (Ry::String& Opt : Args)
		{
			if (Opt.find_first(Option, 0) >= 0)
			{
				return true;
			}
		}

		return false;
	}

	Ry::StringView ParseOption(Ry::ArrayList<Ry::String>& Args, Ry::String Option)
	{
		for (Ry::String& Opt : Args)
		{
			if (Opt.find_first(Option, 0) >= 0)
			{
				int SepIndex = Opt.find_first("=", 0);

				return Opt.substring(SepIndex + 1);
			}
		}

		return StringView(nullptr, 0, 0, 0);
	}
	
}
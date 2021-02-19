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

	Ry::StringView ParseOption(std::vector<Ry::String>& Args, Ry::String Option)
	{
		for (Ry::String& Opt : Args)
		{
			if (Opt.find_first(Option, 0) >= 0)
			{
				int SepIndex = Opt.find_first("=", 0);

				return Opt.substring(SepIndex);
			}
		}

		return StringView(nullptr, 0, 0, 0);
	}
	
}
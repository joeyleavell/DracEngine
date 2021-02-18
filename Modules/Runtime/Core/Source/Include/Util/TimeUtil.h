#pragma once

#include "Core/String.h"
#include <ctime>

namespace Ry
{

	Ry::String GetTimestamp(Ry::String Format)
	{
		char Buffer[1024 * 4];

		time_t Now = time(0);
		struct tm AsTime;

#ifdef MSVC_COMPILER
		localtime_s(&AsTime, &Now);
#else
		AsTime = *localtime(&Now);
#endif
		
		strftime(Buffer, 1024 * 4, Format.getData(), &AsTime);

		return Ry::String(Buffer);
		
		/*Ry::String Result;
		
		// Delimit on colons
		Ry::SharedPtr<StringView[]> Sections;
		int32 Count = Format.split(":", Sections);

		for(int32 SectionIndex = 0; SectionIndex < Count; SectionIndex++)
		{
			Ry::StringView& Section = Sections[SectionIndex];
			int32 FormatCount = Section.getSize();
			
			if(FormatCount > 0)
			{
				char Format = Section[0];

				Ry::String SectionResult;

				// Hours
				if(Format == 'h' || Format == 'H')
				{
					SectionResult = Ry::Printf(Ry::String("%") + Ry::to_string(FormatCount) + "d",);
				}

				// Minutes
				if (Format == 'm' || Format == 'M')
				{

				}

				// Seconds
				if (Format == 's' || Format == 'S')
				{

				}


			}
		}		*/
	}
	
}

#pragma once

#include "Core/String.h"
#include <fstream>
#include <ctime>

namespace Ry
{ 

	enum class Sev
	{
		Normal,
		Warning,
		Error
	};

	CORE_MODULE Ry::String GetTimestamp(Ry::String Format);

	class CORE_MODULE Logger
	{
	public:
		Logger()
		{
		}

		void SetLogOutput(std::string& Output)
		{
		//	FileOutput.open(Output);
		}

		void Log(Ry::String Format, ...)
		{
			va_list List;
			va_start(List, Format);
			this->Log(Sev::Normal, Format, List);
			va_end(List);
		}

		void LogWarn(Ry::String Format, ...)
		{
			va_list List;
			va_start(List, Format);
			this->Log(Sev::Warning, Format, List);
			va_end(List);
		}

		void LogError(Ry::String Format, ...)
		{
			va_list List;
			va_start(List, Format);
			this->Log(Sev::Warning, Format, List);
			va_end(List);
		}

		void Log(Ry::String Format, va_list Args)
		{
			this->Log(Sev::Normal, Format, Args);
		}

		void LogWarn(Ry::String Format, va_list Args)
		{
			this->Log(Sev::Warning, Format, Args);
		}

		void LogError(Ry::String Format, va_list Args)
		{
			this->Log(Sev::Warning, Format, Args);
		}

		void Log(Sev Severity, Ry::String Format, va_list Args)
		{
			std::ostream& OS = (Severity == Sev::Error) ? std::cerr : std::cout;	
			Ry::String Msg = "[" + GetTimestamp("%H:%M:%S") + "]";

			if(Severity == Sev::Warning)
			{
				Msg += "[WARNING]: ";
			}
			else if(Severity == Sev::Error)
			{
				Msg += "[ERROR]: ";
			}
			else
			{
				Msg += ": ";
			}

			Msg += Ry::CreateFormatted(Format, Args) + "\n";

			OS << *Msg;
			
			// if(FileOutput.is_open())
			// {
			// 	FileOutput << *Msg;
			// }
		}

	private:
		//std::ofstream FileOutput;

	};
	
}

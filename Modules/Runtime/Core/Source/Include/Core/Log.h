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

		void Log(Ry::String Format)
		{
			this->Log(Sev::Normal, Format);
		}

		void LogWarn(Ry::String Format)
		{
			this->Log(Sev::Warning, Format);
		}

		void LogError(Ry::String Format)
		{
			this->VariadicLog(Sev::Error, Format, nullptr);
		}

		void Logf(Ry::String Format...)
		{
			va_list List;
			va_start(List, Format);
			this->VariadicLog(Sev::Normal, Format, List);
			va_end(List);
		}

		void LogWarnf(Ry::String Format...)
		{
			va_list List;
			va_start(List, Format);
			this->VariadicLog(Sev::Warning, Format, List);
			va_end(List);
		}

		void LogErrorf(Ry::String Format...)
		{
			va_list List;
			va_start(List, Format);
			this->VariadicLog(Sev::Error, Format, List);
			va_end(List);
		}

		// void LogWarn(Ry::String Format, va_list Args)
		// {
		// 	this->Log(Sev::Warning, Format, Args);
		// }
		//
		// void LogError(Ry::String Format, va_list Args)
		// {
		// 	this->Log(Sev::Error, Format, Args);
		// }

		Ry::String GetFormatted(Sev Severity, Ry::String LogEntry)
		{
			Ry::String Msg = "[" + GetTimestamp("%H:%M:%S") + "]";

			if (Severity == Sev::Warning)
			{
				Msg += "[WARNING]: ";
			}
			else if (Severity == Sev::Error)
			{
				Msg += "[ERROR]: ";
			}
			else
			{
				Msg += ": ";
			}

			Msg += LogEntry;

			return Msg;
		}
		
		
		void Log(Sev Severity, Ry::String LogEntry)
		{
			std::ostream& OS = (Severity == Sev::Error) ? std::cerr : std::cout;
			Ry::String FullLogMsg = GetFormatted(Severity, LogEntry);

			OS << *FullLogMsg << std::endl;

			// if(FileOutput.is_open())
			// {
			// 	FileOutput << *Msg;
			// }
		}
		
		void VariadicLog(Sev Severity, Ry::String Format, va_list Args)
		{
			std::ostream& OS = (Severity == Sev::Error) ? std::cerr : std::cout;	
			Ry::String FullLogMsg = GetFormatted(Severity, Ry::CreateFormatted(Format, Args));

			OS << *FullLogMsg << std::endl;
			
			// if(FileOutput.is_open())
			// {
			// 	FileOutput << *Msg;
			// }
		}

	private:
		//std::ofstream FileOutput;

	};
	
}

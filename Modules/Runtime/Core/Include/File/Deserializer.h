#pragma once

#include <fstream>
#include "Deserializer.gen.h"

namespace Ry
{

	class Deserializer
	{
	public:

		Deserializer();

		void Open(const Ry::String& InputFile);
		void Close();

		uint8 ReadUByte();
		uint16 ReadUShort();
		uint32 ReadUInt();
		uint32 ReadULongInt();
		int8 ReadByte();
		int16 ReadShort();
		int32 ReadInt();
		int32 ReadLongInt();

		Ry::String ReadString();

	private:

		std::ifstream Input;

	};

}

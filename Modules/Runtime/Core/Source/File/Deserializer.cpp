#include "File/Deserializer.h"

namespace Ry
{
	void Deserializer::Open(const Ry::String& InputFile)
	{
		Input.open(*InputFile);
	}

	void Deserializer::Close()
	{
		Input.close();
	}

	uint8 Deserializer::ReadUByte()
	{
		char Num;
		Input.read(&Num, 1);

		return Num;
	}

	uint16 Deserializer::ReadUShort()
	{
		char Num[2];
		Input.read(Num, 2);

		uint16 Result = (Num[0] >> 0) | (Num[1] >> 8);
		return Result;
	}

	uint32 Deserializer::ReadUInt()
	{
		char Num[4];
		Input.read(Num, 4);

		uint16 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24);
		return Result;
	}

	uint32 Deserializer::ReadULongInt()
	{
		char Num[8];
		Input.read(Num, 8);

		uint64 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24) | (Num[4] >> 0) | (Num[5] >> 8) | (Num[6] >> 16) | (Num[7] >> 24);
		return Result;
	}

	int8 Deserializer::ReadByte()
	{
		char Num;
		Input.read(&Num, 1);

		return Num;
	}

	int16 Deserializer::ReadShort()
	{
		char Num[2];
		Input.read(Num, 2);

		int16 Result = (Num[0] >> 0) | (Num[1] >> 8);
		return Result;
	}

	int32 Deserializer::ReadInt()
	{
		char Num[4];
		Input.read(Num, 4);

		int16 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24);
		return Result;
	}

	int32 Deserializer::ReadLongInt()
	{
		char Num[8];
		Input.read(Num, 8);

		int64 Result = (Num[0] >> 0) | (Num[1] >> 8) | (Num[2] >> 16) | (Num[3] >> 24) | (Num[4] >> 0) | (Num[5] >> 8) | (Num[6] >> 16) | (Num[7] >> 24);
		return Result;
	}

	Ry::String Deserializer::ReadString()
	{
		uint32 Size = ReadUInt();
		Ry::String Result(Size);

		for(uint32 Index = 0; Index < Size; Index++)
		{
			char Next = ReadUByte();
			Result[Index] = Next;
		}

		return Result;
	}
}

#pragma once

#include <fstream>
#include "Serializer.gen.h"
#include "Core/Object.h"

namespace Ry
{

	/**
	 * All operations in this class for now assume the system orders numbers in little-endian fashion, and has 8 bits per byte.
	 */
	class CORE_MODULE Serializer
	{
	public:
		Serializer();

		void Open(const Ry::String& OutFile);
		void Close();

		void WriteUByte(uint8 Byte);
		void WriteUShort(uint16 Short);
		void WriteUInt(uint32 Int);
		void WriteULongInt(uint64 LongInt);
		void WriteByte(int8 Byte);
		void WriteShort(int16 Short);
		void WriteInt(int32 Int);
		void WriteLongInt(int64 LongInt);

		void WriteString(const Ry::String& String);

		void WriteObject(const Ry::Object* Obj);

	private:

		// Todo: support custom serialize
		void SerializeField(const Ry::Field& Field, const Ry::Object* Obj);
		void SerializeObjectField(const Ry::Field& Field, const Ry::Object* Obj);
		void SerializeStringField(const Ry::Field& Field, const Ry::Object* Obj);

		template<typename T>
		void SerializeIntField(const Ry::Field& Field, const Ry::Object* Obj)
		{
			const T* ChildNum = Field.GetConstPtrToField<T, Ry::Object>(Obj);
			WriteInt(*ChildNum);
		}

		Ry::String OutFile;

		std::ofstream Output;

	};

}


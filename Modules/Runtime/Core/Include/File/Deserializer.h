#pragma once

#include <fstream>
#include "Core/Globals.h"
#include "Core/Object.h"
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
		uint64 ReadULongInt();
		int8 ReadByte();
		int16 ReadShort();
		int32 ReadInt();
		int64 ReadLongInt();

		Ry::String ReadString();

		Ry::Object* ReadObject();

	private:

		// Todo: support custom deserialize
		void DeserializeField(const Field* Field, Ry::Object* DstObject);

		/**
		 * Generic method to deserialize a field, given a function pointer to a function in this class that takes no arguments.
		 */
		template<typename FieldType>
		void DeserializeField_Helper(const Field* Field, Ry::Object* DstObject, FieldType(Ry::Deserializer::* DeserializeFunction)())
		{
			if (FieldType* PtrToType = Field->GetMutablePtrToField<FieldType>(DstObject))
			{
				// Invoke the passed in deserialize function, this will load the value we want to set in the class
				FieldType StoredObject = (this->*DeserializeFunction)();

				// Set the ptr-to to the stored object we just read
				*PtrToType = StoredObject;
			}
			else
			{
				Ry::Log->LogErrorf("Deserializer::DeserializeField_Helper: Failed to get mutable ptr to field %s in object of class %s", *Field->Name, *DstObject->GetClass()->Name);
			}
		}

		Ry::String InputFile;
		std::ifstream Input;

	};

}

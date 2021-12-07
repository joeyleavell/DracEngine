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

		void IgnoreBytes(uint64 Count);

		// Todo: support custom deserialize
		void DeserializeField(const Field* Field, Ry::Object* DstObject, uint64 FieldSize);

		void DeserializeArrayListField(const Field* Field, Ry::Object* DstObject);

		template<typename ArrayListType>
		void DeserializeArrayListField_Helper(const Field* Field, Ry::Object* DstObject, ArrayListType (Ry::Deserializer::* DeserializeFunction)())
		{
			int32 ElementCount = ReadUInt();

			if (Ry::ArrayList<ArrayListType>* PtrToArrayList = Field->GetMutablePtrToField<Ry::ArrayList<ArrayListType>>(DstObject))
			{
				Ry::ArrayList<ArrayListType> NewArrayList;

				for(int32 Index = 0; Index < ElementCount; Index++)
				{
					// Load the element by calling the deserialize function
					ArrayListType StoredObject = (this->*DeserializeFunction)();

					// Append it to the modifiable array list
					NewArrayList.Add(StoredObject);
				}

				// Set the existing array list
				*PtrToArrayList = NewArrayList;
			}
			else
			{
				Ry::Log->LogErrorf("Deserializer::DeserializeField_Helper: Failed to get mutable ptr to field %s in object of class %s", *Field->Name, *DstObject->GetClass()->Name);
			}
		}


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

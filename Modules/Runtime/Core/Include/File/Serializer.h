#pragma once

#include <fstream>
#include <sstream>
#include <vector>
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
		void WriteBytes(const char* Bytes, uint64 Count);

		void WriteString(const Ry::String& String);

		void WriteObject(const Ry::Object* Obj);

	private:

		inline std::ostream& GetOutStream()
		{
			if(StreamStackSize > 0)
			{
				return StreamStack[StreamStackSize - 1];
			}
			else
			{
				return Output;
			}
		}

		// Todo: support custom serialize
		void SerializeField(const Ry::Field& Field, const Ry::Object* Obj);
		void SerializeObjectField(const Ry::Field& Field, const Ry::Object* Obj);
		void SerializeStringField(const Ry::Field& Field, const Ry::Object* Obj);
		void SerializeArrayField(const Ry::Field& Field, const Ry::Object* Obj);

		template<typename T, typename SerializeParam = T>
		void SerializeArrayField_Helper(const Ry::Field& Field, const Ry::Object* Obj, void (Ry::Serializer::* SerializeFunction)(SerializeParam ToSerialize))
		{
			if(const Ry::ArrayList<T>* ArrayRef = Field.GetConstPtrToField<Ry::ArrayList<T>, Ry::Object>(Obj))
			{
				// Write out the name of the template type to resolve at load time
				WriteString(Field.Type->TemplateTypes[0]->Name);

				// Write out the amount of elements
				WriteUInt(ArrayRef->GetSize());

				// Serialize each element in the array list
				for(int32 Index = 0; Index < ArrayRef->GetSize(); Index++)
				{
					// Pass the current element to the specified serialize function
					(this->*SerializeFunction)((*ArrayRef)[Index]);
				}
			}
		}

		template<typename T>
		void SerializeIntField(const Ry::Field& Field, const Ry::Object* Obj)
		{
			const T* ChildNum = Field.GetConstPtrToField<T, Ry::Object>(Obj);
			WriteInt(*ChildNum);
		}

		void PushFieldStream()
		{
			// Insert a new stream
			//std::ostringstream NewStream;
			//FieldStreamStack.Add(NewStream);

			// Add a new string stream
			//FieldStreamStack.SetSize(FieldStreamStack.GetSize() + 1);

			StreamStackSize++;

			std::ostringstream* PrevStack = StreamStack;
			StreamStack = new std::ostringstream[StreamStackSize];

			// Copy over the old output streams
			for(int32 Index = 0; Index < StreamStackSize - 1; Index++)
			{
				StreamStack[Index] = std::move(PrevStack[Index]);
			}
		}

		std::string PopFieldStream()
		{
			// Pop last element
			std::ostringstream& Top = StreamStack[StreamStackSize - 1];
			std::string Res = Top.str();

			// Delete topmost element
			StreamStackSize--;
			std::ostringstream* PrevStack = StreamStack;

			if(StreamStackSize > 0)
			{
				StreamStack = new std::ostringstream[StreamStackSize];
				// Copy over the old output streams
				for (int32 Index = 0; Index < StreamStackSize; Index++)
				{
					StreamStack[Index] = std::move(PrevStack[Index]);
				}
			}
			else
			{
				StreamStack = nullptr;
			}

			return Res;
		}

		Ry::String OutFile;
		std::ofstream Output;

		int32 StreamStackSize = 0;
		std::ostringstream* StreamStack = nullptr;
	};

}


#include "Core/String.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "Core/Assert.h"
#include "Core/Memory/PoolAllocator.h"

namespace Ry
{

	// 1000 200 byte chunks
	static PoolAllocator* SmallStringAllocator;

	void StringCopy(char* Dst, const char* Src, uint64 DstSize)
	{
#ifdef MSVC_IMPLEMENTATION
		strcpy_s(Dst, DstSize, Src);
#else
		strncpy(Dst, Src, DstSize);
#endif
	}


	void VsPrintf(char* Dst, uint64 MaxWrite, const char* Format, va_list Args)
	{
	#ifdef MSVC_IMPLEMENTATION
			vsprintf_s(Dst, MaxWrite, Format, Args);
	#else
			vsnprintf(Dst, MaxWrite, Format, Args);
	#endif
	}

	StringView StringSubstring(const char* String, uint32 StringLength, uint32 BeginIndex, uint32 EndIndex)
	{
		CORE_ASSERT(BeginIndex >= 0, "String::substring beg must be greater than or equal to zero");
		CORE_ASSERT(BeginIndex < StringLength, "String::substring beg must be less than string size");
		CORE_ASSERT(EndIndex >= 0, "String::substring end must be greater than or equal to zero");
		CORE_ASSERT(EndIndex <= StringLength, "String::substring end must be less than or equal to string size");
		CORE_ASSERT(BeginIndex <= EndIndex, "String::substring beg must be less than or equal to end");

		return { String, StringLength, BeginIndex, EndIndex };
	}

	int32 StringFindFirst(const char* String, uint32 StringLength, const char* SearchString, uint32 SearchStringLength, uint32 BegIndex)
	{
		CORE_ASSERT(BegIndex >= 0, "find_first index must be greater than or equal to zero");
		CORE_ASSERT(BegIndex < StringLength, "find_first index must be less than string size");

		/************************************************************************/
		/* Shortcuts                                                            */
		/************************************************************************/

		// The first empty string is always at the beginning of any string
		if (SearchStringLength < 1)
			return BegIndex;

		// Substring must be smaller for it to occur in this string
		if (SearchStringLength > StringLength)
			return -1;

		// Check if there is enough space for the substring to possibly be in this string
		if (StringLength - BegIndex < SearchStringLength)
			return -1;


		uint32 i = BegIndex;
		uint32 j = 0;
		uint32 at = i;
		int32 found = -1;

		while (i < StringLength && j < SearchStringLength && found < 0)
		{
			if (String[i] == SearchString[j])
			{
				if (j == 0)
				{
					at = i;
				}

				j++;

				if (j == SearchStringLength)
				{
					found = at;
				}
			}
			else
			{
				j = 0;
				at = i;
			}

			i++;
		}

		return found;
	}

	int32 StringSplit(const char* String, uint32 StringLength, const char* Delim, uint32 DelimLength, StringView** Result)
	{
		CORE_ASSERT(DelimLength > 0, "String::split size of delimiter must be greater than zero");

		if (StringLength == 0)
		{
			Result = nullptr;
			return 0;
		}

		// There can not be more delimiters found than the size of the string (unless, of course, we allow empty strings which we're not)
		// Also, we're allocating the delimiters on the stack for small strings for efficiency
		uint32* delims = new uint32[(uint64) StringLength + 2];

		// Set the first delimited index as the beginning of the string
		uint32 delim = 0;

		// i represents the end of the last delimiter
		uint32 i = 0;

		while (i < StringLength)
		{
			int32 next = StringFindFirst(String, StringLength, Delim, DelimLength, i);

			delims[delim] = i;

			if (next >= 0)
			{
				delims[delim + 1] = next;
				i = next + DelimLength;
			}
			else
			{
				// Once we can't find any more delimiters, the last delimited index is the end of the string.
				delims[delim + 1] = (i = StringLength);
			}

			delim += 2;
		}

		int32 strings = delim / 2;
		*Result = new StringView[strings];

		for (uint32 i = 0, j = 0; i < delim - 1; i += 2, j++)
		{
			uint32 this_delim = delims[i];
			uint32 next_delim = delims[i + 1];

			(*Result)[j] = StringSubstring(String, StringLength, this_delim, next_delim);
		}

		delete[] delims;

		return delim / 2;
	}

	StringView TrimString(const char* String, uint32 StringLength)
	{
		uint32 TrimBeg = 0;
		uint32 TrimEnd = StringLength - 1;

		while (TrimBeg < StringLength && std::isspace(String[TrimBeg]))
		{
			TrimBeg++;
		}

		while (TrimEnd >= 0 && std::isspace(String[TrimEnd]))
		{
			TrimEnd--;
		}

		return StringSubstring(String, StringLength, TrimBeg, TrimEnd + 1);
	}

	float ParseFloat(const char* String, uint32 StringLength)
	{
		return (float) atof(String);
	}

	double ParseDouble(const char* String, uint32 StringLength)
	{
		return atof(String);
	}

	bool ParseBool(const char* String, uint32 StringLength)
	{
		if (strcmp(String, "true") == 0)
			return true;
		if (strcmp(String, "false") == 0)
			return false;
		if (strcmp(String, "True") == 0)
			return true;
		if (strcmp(String, "False") == 0)
			return false;
		if (strcmp(String, "TRUE") == 0)
			return true;
		if (strcmp(String, "FALSE") == 0)
			return false;
		if (strcmp(String, "1") == 0)
			return true;
		if (strcmp(String, "0") == 0)
			return false;
		if (strcmp(String, "t") == 0)
			return true;
		if (strcmp(String, "f") == 0)
			return false;
		if (strcmp(String, "T") == 0)
			return true;
		if (strcmp(String, "F") == 0)
			return false;

		return false;
	}

	int32 ParseInt(const char* String, uint32 StringLength)
	{
		return atoi(String);
	}

	uint32 ParseUnsignedInt(const char* String, uint32 StringLength)
	{
		return (uint32) atoi(String);
	}

	String to_string(int64 a)
	{
		return String(std::to_string(a).c_str());
	}

	String to_string(uint64 a)
	{
		return String(std::to_string(a).c_str());
	}

	String to_string(uint32 a)
	{
		return String(std::to_string(a).c_str());
	}

	String to_string(int32 a)
	{
		return String(std::to_string(a).c_str());
	}

	String to_string(double a)
	{
		return String(std::to_string(a).c_str());
	}

	String to_string(float a)
	{
		return String(std::to_string(a).c_str());
	}

	String to_string(bool a)
	{
		return String(std::to_string(a).c_str());
	}

	String::String():
	data(nullptr)
	{
		AllocData(0);
	}

	String::String(const char* dat)
	{
		AllocData(strlen(dat));
		StringCopy(data, dat, (uint64) size + 1);
	}

	String::String(const char* InData, int32 Size)
	{
		// Allocate
		AllocData(Size);
		Ry::StringCopy(data, InData, Size + 1);
	}

	String::String(const String& other)
	{
		AllocData(other.size);
		Ry::StringCopy(data, other.data, size + 1);
	}

	String::String(String&& Other) noexcept
	{
		this->size = Other.size;
		this->data = Other.data;
		this->bStackAllocated = Other.bStackAllocated;
		Other.data = nullptr;
	}

	String::String(const StringView& View):
	data(nullptr),
	size(0)
	{
		AllocData(View.getSize());
		for (uint32 i = 0; i < View.getSize(); i++)
		{
			data[i] = View[i];
		}
		data[size] = '\0';
	}

	String::String(uint32 AllocSize):
	data(nullptr),
	size(0)
	{
		AllocData(AllocSize);
	}

	String::~String()
	{
		FreeData();
	}

	char* String::getData() const
	{
		return data;
	}

	uint32 String::getSize() const
	{
		return size;
	}

	String String::to_lower() const
	{
		String Res(size);

		for (uint32 Character = 0; Character < size; Character++)
		{
			Res[Character] = tolower((int)data[Character]);
		}

		Res[size] = '\0';

		return Res;
	}

	String& String::operator=(const String& other)
	{
		if (this == &other)
			return *this;

		FreeData();
		
		AllocData(other.size);
		
		Ry::StringCopy(data, other.data, size + 1);

		return *this;
	}

	String& String::operator=(String&& Other) noexcept
	{
		FreeData();

		this->data = Other.data;
		this->size = Other.size;
		this->bStackAllocated = Other.bStackAllocated;
		Other.data = nullptr;

		return *this;
	}

	bool String::operator<(const String& other) const
	{
		for(uint32 Index = 0; Index < size && Index < other.size; Index++)
		{
			if (data[Index] < other.data[Index])
				return true;
			else if (data[Index] > other.data[Index])
				return false;
		}

		// Check which string is a prefix of the other

		if (size < other.size) // This string is a prefix
			return true;
		return false; // The other string is a prefix
	}

	bool String::operator==(const String& other) const
	{
		if (size != other.size)
			return false;

		return this->operator==(other.data);
	}

	bool String::operator==(const char* other) const
	{

		if (size != strlen(other))
			return false;

		if (size == 0)
		{
			if (other[0] == '\0')
				return true;
			return false;
		}

		for (uint32 i = 0; i < size; i++)
		{
			if (data[i] != other[i])
				return false;
		}

		return true;
	}

	bool String::operator!=(const String& other) const
	{
		return !(*this == other);
	}

	bool String::operator!=(const char* other) const
	{
		return !(*this == other);
	}

	String String::operator+(const String& other) const
	{
		Ry::String Result(size + other.size);
		for (uint32 i = 0; i < size; i++)
			Result[i] = data[i];
		for (uint32 i = size; i < size + other.size; i++)
			Result[i] = other.data[i - size];
		Result[size + other.size] = '\0';

		return Result;
	}

	String operator+(const StringView& View, const String& b)
	{
		uint32 ASize = View.getSize();
		String Result(ASize + b.size);
		
		for (uint32 i = 0; i < ASize; i++)
			Result[i] = View[i];
		for (uint32 i = ASize; i < ASize + b.size; i++)
			Result[i] = b.data[i - ASize];
		Result[ASize + b.size] = '\0';

		return Result;
	}

	String operator+(const String& A, const StringView& View)
	{
		int32 BSize = View.getSize();
		String Result( A.size + BSize);

		for (uint32 i = 0; i < A.size; i++)
			Result[i] = A[i];
		for (uint32 i = A.size; i < A.size + BSize; i++)
			Result[i] = View[i - A.size];
		Result[A.size + BSize] = '\0';

		return Result;
	}

	String& String::operator+=(const String& other)
	{
		const_cast<String*>(this)->operator=(*this + other);
		
		return *this;
	}

	double String::to_double() const
	{
		return std::stod(data);
	}

	float String::to_float() const
	{
		return std::stof(data);
	}
	
	int32 String::to_int32() const
	{
		return std::stoi(data);
	}
	
	uint32 String::to_uint32() const
	{
		return (uint32) std::stoul(data);
	}
	
	bool String::to_bool() const
	{
		return ParseBool(data, size);
	}

	String operator+(const char* a, const String& b)
	{
		return String(a) + b;
	}

	char& String::operator[](uint32 index) const
	{
		return data[index];
	}

	String String::operator+(const char* Other) const
	{
		int32 OtherSize = strlen(Other);
		String Res(size + strlen(Other));
		for (uint32 i = 0; i < size; i++)
			Res[i] = data[i];
		for (uint32 i = size; i < size + OtherSize; i++)
			Res[i] = Other[i - size];

		Res[size + OtherSize] = '\0';
		return Res;
	}

	char* String::operator*() const
	{
		return data;
	}

	bool String::IsEmpty() const
	{
		return size <= 0;
	}

	int32 String::split(const String& str, StringView** Result) const
	{
		return StringSplit(data, size, str.data, str.size, Result);
	}

	StringView String::substring(uint32 beg) const
	{
		return substring(beg, getSize());
	}

	StringView String::substring(uint32 beg, uint32 end) const
	{
		return StringSubstring(data, size, beg, end);
	}

	String String::right_most(uint32 count) const
	{
		CORE_ASSERT(count <= getSize(), "right_most count must be less than or equal to string size");

		if (count == getSize())
			return *this;

		String Res(count);
		for (uint32 i = 0; i < count; i++)
			Res[i] = data[i + (getSize() - count)];
		Res[count] = '\0';

		return String(Res);
	}

	String String::left_most(uint32 count) const
	{
		CORE_ASSERT(count <= getSize(), "left_most count must be less than or equal to string size");

		if (count == getSize())
			return *this;

		String Res(count);
		for (uint32 i = 0; i < count; i++)
			Res[i] = data[i];
		Res[count] = '\0';

		return Res;
	}

	int32 String::find_first(const String& str, uint32 index) const
	{
		return StringFindFirst(data, size, str.data, str.size, index);
	}

	int32 String::find_last(const String& str, uint32 index) const
	{
		CORE_ASSERTF(index >= 0, "find_last index must be greater than or equal to zero");
		CORE_ASSERTF(index < getSize(), "find_last index must be less than string size");

		/************************************************************************/
		/* Shortcuts                                                            */
		/************************************************************************/

		// The first empty string is always at the beginning of any string
		if (str.getSize() < 1)
			return index;

		// Substring must be smaller for it to occur in this string
		if (str.getSize() > getSize())
			return -1;

		// Check if there is enough space for the substring to possibly be in this string
		if (getSize() - index - 1 < str.getSize())
			return -1;

		int32 i = getSize() - index - 1;
		int32 j = str.getSize() - 1;
		int32 found = -1;

		while (i >= 0 && j >= 0 && found < 0)
		{
			if (data[i] == str[j])
			{
				j--;

				if (j < 0)
				{
					found = i;
				}
			}
			else
			{
				j = str.getSize() - 1;
			}

			i--;
		}

		return found;
	}

	void String::Replace(char Original, char New)
	{
		for(int32 Index = 0; Index < size; Index++)
		{
			if(data[Index] == Original)
			{
				data[Index] = New;
			}
		}
		
	}

	Ry::StringView String::Trim()
	{
		return TrimString(data, size);
	}

	void String::AllocData(uint32 Size)
	{
		if (!SmallStringAllocator)
			SmallStringAllocator = new PoolAllocator(SMALL_STRING_SIZE, 10000);

		this->size = Size;

		int32 ActualSize = Size + 1; // Account for null terminator
		
#if ENABLE_SSO
		if(ActualSize <= SMALL_STRING_SIZE)
		{
			data = (char*) SmallStringAllocator->Allocate();
			bStackAllocated = true;
		}
		else
#endif
		{
			bStackAllocated = false;
			data = new char[ActualSize];
		}

		data[0] = '\0';		
	}

	void String::FreeData()
	{
		if(data)
		{
			if (bStackAllocated)
				SmallStringAllocator->Free(data);
			else
				delete[] data;

			data = nullptr;
		}
		
	}

	String CreateFormatted(Ry::String Format, va_list ArgsList)
	{
		char Buffer[PRINTF_BUFFER_SIZE];

		// Printf to the buffer
		VsPrintf(Buffer, PRINTF_BUFFER_SIZE - 1, Format.getData(), ArgsList);

		Buffer[PRINTF_BUFFER_SIZE - 1] = '\0';

		// Construct a string from the buffer
		return Ry::String(Buffer);
	}
	
}

void FreeArray(Ry::String* arrayPtr)
{
	delete[] arrayPtr;
}
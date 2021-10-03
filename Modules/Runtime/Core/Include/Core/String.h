#pragma once

#include <iostream>
#include "Core/Platform.h"
#include <cstring>
#include <cstdarg>

#define PRINTF_BUFFER_SIZE (5*1024)

#define ENABLE_SSO 1
#define SMALL_STRING_SIZE 200

namespace Ry
{	
	class CORE_MODULE StringView;

	void StringCopy(char* Dst, const char* Src, uint64 DstSize);
	void VsPrintf(char* Dst, uint64 MaxWrite, const char* Format, va_list Args);

	// Standard string functions on c strings
	CORE_MODULE StringView StringSubstring(const char* String, uint32 StringLength, uint32 BeginIndex, uint32 EndIndex);
	CORE_MODULE int32 StringFindFirst(const char* String, uint32 StringLength, const char* SearchString, uint32 SearchStringLength, uint32 BegIndex);
	CORE_MODULE int32 StringSplit(const char* String, uint32 StringLength, const char* Delim, uint32 DelimLength, StringView** Result);
	CORE_MODULE StringView TrimString(const char* String, uint32 StringLength);

	// Parse functions for standard c strings
	CORE_MODULE float ParseFloat(const char* String);
	CORE_MODULE double ParseDouble(const char* String);
	CORE_MODULE int32 ParseInt(const char* String);
	CORE_MODULE uint32 ParseUnsignedInt(const char* String);
	CORE_MODULE bool ParseBool(const char* String);

	class CORE_MODULE StringView
	{
		
	public:

		StringView():
		Parent(nullptr),
		ParentSize(0),
		Size(0),
		Begin(0),
		End(0)
		{
			
		}

		StringView(const char* Parent, uint32 ParentSize, uint32 Begin, uint32 End):
		Parent(Parent),
		ParentSize(ParentSize),
		Begin(Begin),
		End(End)
		{
			this->Size = End - Begin;			
		}

		StringView(const StringView& Other)
		{
			this->Parent = Other.Parent;
			this->ParentSize = Other.ParentSize;
			this->Begin = Other.Begin;
			this->End = Other.End;
			this->Size = Other.Size;
		}

		StringView(StringView&& Other) noexcept
		{
			this->Parent = Other.Parent;
			this->ParentSize = Other.ParentSize;
			this->Begin = Other.Begin;
			this->End = Other.End;
			this->Size = Other.Size;
		}
		
		void Set(const char* Parent, uint32 ParentSize, uint32 Begin, uint32 End)
		{
			this->Parent = Parent;
			this->ParentSize = ParentSize;
			this->Begin = Begin;
			this->End = End;
			this->Size = End - Begin;
		}
		
		uint32 getSize() const
		{
			return Size;
		}

		const char* GetParent() const
		{
			return Parent;
		}

		const char* GetData()
		{
			return Parent + Begin;
		}

		uint32 GetBegin() const
		{
			return Begin;
		}

		uint32 GetEnd() const
		{
			return End;
		}

		bool IsEmpty() const
		{
			return Size == 0;
		}

		char operator[](uint32 Index) const
		{
			return Parent[Begin + Index];
		}

		bool operator==(const char* Other) const
		{
			if (Size != strlen(Other))
				return false;
			
			return strncmp(Parent + Begin, Other, Size) == 0;
		}

		int32 split(const char* Delim, StringView** Result) const
		{
			return StringSplit(Parent + Begin, (size_t) Size, Delim, (uint32) strlen(Delim), Result);
		}

		StringView substring(uint32 beg) const
		{
			return substring(beg, End);
		}

		StringView substring(uint32 beg, uint32 end) const
		{
			return StringSubstring(Parent + Begin, Size, beg, end);
		}

		StringView Trim()
		{
			return TrimString(Parent + Begin, Size);
		}

		float to_float() const
		{
			return ParseFloat(Parent + Begin);
		}

		int32 to_int32() const
		{
			return ParseInt(Parent + Begin);
		}

		int32 to_uint32() const
		{
			return ParseUnsignedInt(Parent + Begin);
		}

		bool to_bool() const
		{
			return ParseBool(Parent + Begin);
		}

		StringView& operator=(const StringView& Other)
		{
			if (this == &Other)
				return *this;
			
			this->Parent = Other.Parent;
			this->ParentSize = Other.ParentSize;
			this->Begin = Other.Begin;
			this->End = Other.End;
			this->Size = Other.Size;

			return *this;
		}

		StringView& operator=(StringView&& Other) noexcept
		{
			this->Parent = Other.Parent;
			this->ParentSize = Other.ParentSize;
			this->Begin = Other.Begin;
			this->End = Other.End;
			this->Size = Other.Size;

			return *this;
		}

	private:
		const char* Parent;
		uint32 ParentSize;

		uint32 Size;
		uint32 Begin;
		uint32 End;


	};

	/**
	 * Engine implementation of string.
	 */
	class CORE_MODULE String
	{

	public:

		String();
		String(const char* dat);
		String(const char* Data, int32 Size);
		String(const String& other);
		String(String&& Other) noexcept;
		String(const StringView& View);		
		String(uint32 AllocSize);
		~String();

		/**
		 * Returns raw c string representation.
		 */
		char* getData() const;

		/**
		 * The amount of characters contained in this string, not including the null terminator.
		 */
		uint32 getSize() const;

		/**
		* @return String The same string, will all charcters in lower case.
		*/
		String to_lower() const;

		String& operator=(const String& other);
		String& operator=(String&& Other) noexcept;

		uint32 operator()() const;
		bool operator<(const String& other) const;
		bool operator==(const String& other) const;
		bool operator==(const char* other) const;
		bool operator!=(const String& other) const;
		bool operator!=(const char* other) const;

		/**
		 * Standard string concatenation.
		 */
		String operator+(const String& other) const;
		
		/**
		 * Standard string concatenation with a c string.
		 */
		String operator+(const char* other) const;

		String operator+(char Other) const;
		
		/**
		 * 
		 */
		String& operator+=(const String& other);

		/**
		 * Converts this string to a double.
		 * @return double The double representation.
		 */
		double to_double() const;
		
		/**
		 * Converts this string to a float.
		 * @return float The float representation.
		 */
		float to_float() const;

		/**
		 * Converts this string to a 32 bit signed int.
		 * @return int32 The int32 representation.
		 */
		int32 to_int32() const;
		
		/**
		* Converts this string to a 64 bit unsigned int.
		* @return uint32 The uint32 representation.
		*/
		uint32 to_uint32() const;

		/**
		* Converts this string to a boolean.
		* The following are parsed as true: True, TRUE, true, T, 1
		* Everything else is interpreted as false.
		* @return bool The boolean representation.
		*/
		bool to_bool() const;

		/**
		 * Overloading for adding a string to a c string and returning an engine string.
		 * @param char* A c string
		 * @param String An engine string
		 * @return String The resulting engine string
		 */
		CORE_MODULE friend String operator+(const char* a, const String& b);
		CORE_MODULE friend String operator+(const StringView& View, const String& b);
		CORE_MODULE friend String operator+(const String& String, const StringView& View);
		CORE_MODULE friend String operator+(char A, const String& B);

		/**
		 * Retrieves the character at the specified index. Undefined results if index is out of bounds.
		 * @param index The index of the string
		 * @return char The character at the index
		 */
		char& operator[](uint32 index) const;

		/**
		 * Overloaded dereference operator to retieve the c string representation.
		 * @return char* The c string representation
		 */
		char* operator*() const;

		bool IsEmpty() const;

		/**
		 * Splits a string up into substrings that were delimited by the specified parameter.
		 *
		 * @param str The delimiter to use to split the string.
		 * @param result A pointer to an uninstantiated array.
		 * @return int32 The count of strings that resulted from the split operation.
		 */
		int32 split(const String& str, StringView** Result) const;
	
		/**
		 * Retrieves a substring contained within this string.
		 *
		 *
		 * @param beg The beginning index of the string.
		 * @param end The ending index of the substring, non inclusive of the element at that index.
		 * @return String the substring contained within this string.
		 */
		StringView substring(uint32 beg) const;
		StringView substring(uint32 beg, uint32 end) const;

		/**
		 * Returns the right most specified amount of characters.
		 * @param count The amount of characters
		 * @return The resulting string
		 */
		String right_most(uint32 count) const;
		
		/**
		 * Returns the left most specified amount of characters.
		 * @param count The amount of characters
		 * @return The resulting string
		 */
		String left_most(uint32 count) const;

		/**
		 * Finds the first occurance of a substring.
		 * @param str The substring to search for
		 * @param index The index starting from the left from which to start the search.
		 * @return int32 The index of where the substring occurs, or -1 if it does not occur
		 */
		int32 find_first(const String& str, uint32 index) const;

		/**
		 * Finds the last occurance of a substring.
		 * @param str The substring to search for
		 * @param index The index starting from the right from which to start the search.
		 * @return int32 The index of where the substring occurs, or -1 if it does not occur
		 */
		int32 find_last(const String& str, uint32 index) const;

		void Replace(char Original, char New);

		Ry::StringView Trim();

	private:

		void AllocData(uint32 Size);
		void FreeData();

		bool bStackAllocated = false;
		char* data = nullptr;
		
		uint32 size;

	};

	// To String functions
	CORE_MODULE String to_string(int64 a);
	CORE_MODULE String to_string(uint64 a);
	CORE_MODULE String to_string(uint32 a);
	CORE_MODULE String to_string(int32 a);
	CORE_MODULE String to_string(double a);
	CORE_MODULE String to_string(float a);
	CORE_MODULE String to_string(bool a);


	CORE_MODULE String CreateFormatted(Ry::String Format, va_list ArgsList);

}

// Cross-DLL memory compatibility
CORE_MODULE void FreeArray(Ry::String* arrayPtr);
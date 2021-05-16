#pragma once

#include <cassert>

namespace Ry
{

	constexpr int32 DEFAULT_ALLOC = 15;
	constexpr int32 DEFAULT_EXPANSION = 100;

	template <typename T>
	class EXPORT_ONLY ArrayList
	{
	public:

		class ConstIterator
		{
		public:
			ConstIterator(const ArrayList<T>& ParentList, int32 Index) :
				ParentList(ParentList),
				CurrentIndex(Index)
			{

			}
			
			const T& operator*() const
			{
				return ParentList[CurrentIndex];
			}

			int32 GetIndex() const
			{
				return CurrentIndex;
			}

			operator bool() const
			{
				return CurrentIndex >= 0 && CurrentIndex < ParentList.GetSize();
			}

			void operator++()
			{
				CurrentIndex++;
			}

			bool operator==(const typename ArrayList<T>::ConstIterator& Other) const
			{
				return CurrentIndex == Other.GetIndex();
			}

		private:
			const ArrayList<T>& ParentList;
			int32 CurrentIndex;
		};

		class Iterator
		{
		public:
			Iterator(const ArrayList<T>& ParentList, int32 Index):
			ParentList(ParentList),
			CurrentIndex(Index)
			{
				
			}

			T& operator*() const
			{
				return ParentList[CurrentIndex];
			}

			int32 GetIndex() const
			{
				return CurrentIndex;
			}

			operator bool() const
			{
				return CurrentIndex >= 0 && CurrentIndex < ParentList.GetSize();
			}

			void operator++()
			{
				CurrentIndex++;
			}

			bool operator==(const typename ArrayList<T>::Iterator& Other) const
			{
				return CurrentIndex == Other.GetIndex();
			}

		private:
			const ArrayList<T>& ParentList;
			int32 CurrentIndex;
		};

		ArrayList():
		AllocatedSize(DEFAULT_ALLOC),
		Size(0)
		{
			Data = new T[DEFAULT_ALLOC];
		}

		ArrayList(int32 Reserve) :
		AllocatedSize(Reserve),
		Size(0)
		{
			Data = new T[Reserve];
		}

		ArrayList(std::initializer_list<T> Init)
		{
			this->Data = new T[DEFAULT_ALLOC];
			this->AllocatedSize = DEFAULT_ALLOC;
			this->Size = 0;

			for(const T& El : Init)
			{
				Add(El);
			}
		}

		ArrayList(const ArrayList<T>& Other)
		{
			CopyFrom(Other);
		}

		~ArrayList()
		{
			delete[] Data;
		}

		T* CopyData()
		{
			if(Size == 0)
			{
				return nullptr;
			}
			
			T* NewData = new T[Size];
			for(int32 Index = 0; Index < Size; Index++)
			{
				NewData[Index] = Data[Index];
			}

			return NewData;			
		}

		T& Last()
		{
			return Data[Size - 1];
		}

		T& First()
		{
			return Data[0];
		}

		int32 GetAllocatedSize() const
		{
			return AllocatedSize;
		}

		int32 GetSize() const
		{
			return Size;
		}

		typename ArrayList<T>::Iterator ListIterator()
		{
			return ArrayList<T>::Iterator(*this, 0);
		}

		void Add(const T& Item)
		{
			// Check if we need to resize
			if(Size >= AllocatedSize)
			{
				Resize((int32) (AllocatedSize * 1.5) + 1);
			}

			// Store element
			Data[Size] = Item;

			// Size increased
			Size++;
		}

		bool Contains(const T& Item) const
		{
			return IndexOf(Item) >= 0;
		}

		int32 IndexOf(const T& Item) const
		{
			// Search for the item
			for (int32 Index = 0; Index < Size; Index++)
			{
				if (Data[Index] == Item)
				{
					return Index;
				}
			}

			return -1;
		}

		void Remove(const T& Item)
		{
			int32 FoundIndex = IndexOf(Item);

			if(FoundIndex >= 0)
			{
				RemoveAt(FoundIndex);
			}
		}

		void RemoveAt(int32 Index)
		{
			assert(Index >= 0 && Index < Size);
				
			// Shift data down to maintain contiguous nature of array
			int32 ShiftIndex = Index;
			while (ShiftIndex < Size - 1)
			{
				Data[ShiftIndex] = Data[ShiftIndex + 1];
				ShiftIndex++;
			}

			Size--;
		}

		bool IsEmpty() const
		{
			return Size <= 0;
		}

		void Clear()
		{
			// todo: delete data, needed for shared ptrs etc
			Size = 0;
		}

		T* GetData() const
		{
			return Data;
		}

		T& operator[](int32 Index) const
		{
			// TODO: assert index in bounds
			
			return Data[Index];
		}

		ArrayList<T>& operator=(const ArrayList<T>& Other)
		{
			if (this == &Other)
				return *this;
			
			CopyFrom(Other);
			return *this;
		}

		void CopyFrom(const ArrayList<T>& Other)
		{
			this->Data = new T[Other.Size];
			this->AllocatedSize = Other.Size;
			this->Size = Other.Size;
			for(int32 El = 0; El < Other.Size; El++)
			{
				this->Data[El] = Other.Data[El];
			}
		}

		// Functions that have to be implemented for a range based for loop
		typename ArrayList<T>::ConstIterator begin() const
		{
			return ArrayList<T>::ConstIterator(*this, 0);
		}

		typename ArrayList<T>::ConstIterator end() const
		{
			return ArrayList<T>::ConstIterator(*this, Size);
		}
		
		typename ArrayList<T>::Iterator begin()
		{
			return ArrayList<T>::Iterator(*this, 0);
		}

		typename ArrayList<T>::Iterator end()
		{
			return ArrayList<T>::Iterator(*this, Size);
		}

		void Resize(int32 NewSize)
		{
			// Allocate with new size
			T* NewArray = new T[NewSize];

			// Copy old values into the new array
			for (int32 Index = 0; Index < Size && Index < NewSize; Index++)
			{
				NewArray[Index] = Data[Index];
			}

			// Account for shrinking elements out of existence
			if (NewSize < Size)
			{
				Size = NewSize;
			}

			delete[] Data;

			// Reassign to new array
			this->Data = NewArray;
			this->AllocatedSize = NewSize;
		}

	private:

		/**
		 * How many elements we actually allocated
		 */
		int32 AllocatedSize;

		/**
		 * How many elements are actually in the array
		 */
		int32 Size;

		// Dynamic array of T
		T* Data;
	};
	
}

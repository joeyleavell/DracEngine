#pragma once

#include "Algorithm/Algorithm.h"
#include <iostream>

#define TABLE_SIZE 5000

namespace Ry
{
	template <class K>
	struct EXPORT_ONLY SetChain
	{
		K Key;
		SetChain<K>* Next;

		SetChain() :
		Key(),
		Next(nullptr)
		{

		}
	};

	template<class K>
	class EXPORT_ONLY SetIterator
	{
	public:

		SetIterator(const SetIterator& Other) :
			HashTable(Other.HashTable),
			TableIndex(Other.TableIndex),
			CurrentEntry(Other.CurrentEntry),
			NextEntry(Other.NextEntry),
			NextIndex(Other.NextIndex)
		{
		}

		SetIterator(const SetIterator&& Other) noexcept :
			HashTable(Other.HashTable),
			TableIndex(Other.TableIndex),
			CurrentEntry(Other.CurrentEntry),
			NextEntry(Other.NextEntry),
			NextIndex(Other.NextIndex)
		{
		}

		SetIterator() :
			HashTable(nullptr),
			TableIndex(-1),
			CurrentEntry(nullptr),
			NextEntry(nullptr),
			NextIndex(-1)
		{
		}

		SetIterator(SetChain<K>** HashTable) :
			HashTable(HashTable),
			TableIndex(-1),
			CurrentEntry(nullptr),
			NextEntry(nullptr),
			NextIndex(-1)
		{
			CurrentEntry = FindNext(TableIndex, -1);

			if (TableIndex >= 0)
			{
				NextEntry = FindNext(NextIndex, TableIndex + 1);
			}
			else
			{
				NextEntry = nullptr;
				NextIndex = -1;
			}

		}

		virtual ~SetIterator() = default;

		explicit operator bool() const
		{
			return CurrentEntry != nullptr;
		}

		bool operator!=(const typename SetIterator<K>& Other) const
		{
			return !(*this == Other);
		}

		bool operator==(const typename SetIterator<K>& Other) const
		{
			if(CurrentEntry && Other.CurrentEntry)
			{
				return CurrentEntry->Key == Other.CurrentEntry->Key;
			}
			else if(CurrentEntry && !Other.CurrentEntry)
			{
				return false;
			}
			else if (!CurrentEntry && Other.CurrentEntry)
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		K operator*() const
		{
			CORE_ASSERTF(TableIndex < TABLE_SIZE, "Tryed to dereference an invalid iterator");
			
			return CurrentEntry->Key;
		}

		K* Key() const
		{
			return operator*();
		}

		SetIterator& operator++()
		{
			// Check if there is another table entry at this index (collision)
			if (CurrentEntry->Next)
			{
				CurrentEntry = CurrentEntry->Next;
			}
			else
			{
				CurrentEntry = NextEntry;
				TableIndex = NextIndex;

				if (TableIndex >= 0)
				{
					NextEntry = FindNext(NextIndex, TableIndex + 1);
				}
				else
				{
					NextEntry = nullptr;
					NextIndex = -1;
				}
			}

			return *this;
		}

		SetIterator<K>& operator=(const SetIterator<K>& Other)
		{
			if (this == &Other)
				return *this;

			this->HashTable = Other.HashTable;
			this->TableIndex = Other.TableIndex;
			this->CurrentEntry = Other.CurrentEntry;
			this->NextEntry = Other.NextEntry;
			this->NextIndex = Other.NextIndex;

			return *this;
		}

		SetIterator<K>& operator=(SetIterator<K>&& Other) noexcept
		{
			this->HashTable = Other.HashTable;
			this->TableIndex = Other.TableIndex;
			this->CurrentEntry = Other.CurrentEntry;
			this->NextEntry = Other.NextEntry;
			this->NextIndex = Other.NextIndex;

			return *this;
		}


	private:

		/**
		 * Finds the next available entry in a hash map.
		 *
		 * @param StartPoint The entry to use as a starting point. This value can be null, in which case we'll just start searching the hash map.
		 * @param StartPointIndex Where to start searching in the hash map.
		 * @param OutIndex The spot to store the index of the next found entry. If the entry is the next link in the chain, the out index remains as the start point index.
		 * @return The next entry in the hash map.
		 */
		SetChain<K>* FindNext(int32& OutIndex, int32 StartPointIndex)
		{
			int32 Index = StartPointIndex + 1;

			// Scan for the next table entry
			bool Found = false;
			while (!Found && Index < TABLE_SIZE)
			{
				if (HashTable[Index])
				{
					Found = true;
				}
				else
				{
					++Index;
				}

			}

			if (Found)
			{
				OutIndex = Index;
				return HashTable[Index];
			}
			else
			{
				OutIndex = -1;
				return nullptr;
			}

		}

		SetChain<K>** HashTable;

		// These values can be negative, indicating no next value.
		// The next value is stored as a convenience to allow explicit checking
		int32 TableIndex;
		int32 NextIndex;

		SetChain<K>* CurrentEntry;
		SetChain<K>* NextEntry;

	};

	template <class K>
	class EXPORT_ONLY Set
	{
	public:

		Set()
		{
			for (uint32 i = 0; i < TABLE_SIZE; i++)
				Table[i] = nullptr;
			Elements = 0;
		}

		Set(const Set<K>& Other)
		{
			for (uint32 i = 0; i < TABLE_SIZE; i++)
				Table[i] = nullptr;
			Elements = 0;

			Copy(Other);
		}

		SetIterator<K> CreateIterator()
		{
			SetIterator<K> KeyItr(Table);
			return KeyItr;
		}

		void Clear()
		{
			for (int32 TableIndex = 0; TableIndex < TABLE_SIZE; TableIndex++)
			{
				SetChain<K>* Chain = Table [TableIndex];
				while (Chain)
				{
					SetChain<K>* Next = Chain->Next;

					delete Chain;
					Chain = Next;
				}

				Table[TableIndex] = nullptr;
			}
		}

		bool IsEmpty()
		{
			
		}

		bool Contains(const K& key) const
		{
			uint32 HashValue = Ry::Hash(key);
			uint32 Bucket = HashValue % TABLE_SIZE;
			SetChain<K>* Chain = Table[Bucket];

			if (Chain == nullptr)
				return false;

			while (Chain != nullptr && !(Chain->Key == key))
				Chain = Chain->Next;

			if (Chain == nullptr)
				return false;
			else
				return true;
		}

		void Insert(const K& Value)
		{
			uint32 HashValue = Ry::Hash(Value);
			uint32 Bucket = HashValue % TABLE_SIZE;

			// Travel to the last element in the bucket
			// OR overwrite key if inserting duplicate
			SetChain<K>* Head     = Table[Bucket];
			SetChain<K>* HeadPrev = nullptr;

			while (Head != nullptr)
			{
				// Found existing element, skip
				if (Head->Key == Value)
				{
					return;
				}

				HeadPrev = Head;
				Head = Head->Next;
			}

			SetChain<K>* NewChain = new SetChain<K>();
			NewChain->Key = Value;
			NewChain->Next = nullptr;

			if (HeadPrev == nullptr)
			{
				Table[Bucket] = NewChain;
			}
			else
			{
				HeadPrev->Next = NewChain;
			}

			Elements++;

		}

		void Remove(const K& Value)
		{
			uint32 HashValue = Ry::Hash(Value);
			uint32 Bucket = HashValue % TABLE_SIZE;
			SetChain<K>* Chain = Table[Bucket];

			if(Chain)
			{
				if (Chain->Next == nullptr)
				{
					if (Chain->Key == Value)
					{
						delete Table[Bucket];
						Table[Bucket] = nullptr;
						Elements--;
					}
				}
				else if(Chain->Key == Value)
				{
					Table[Bucket] = Chain->Next;
					delete Chain;
					Elements--;
				}
				else
				{
					SetChain<K>* PrevChain = Table[Bucket];
					Chain = Chain->Next;

					while (Chain != nullptr && !(Chain->Key == Value))
					{
						PrevChain = Chain;
						Chain = Chain->Next;
					}

					if (Chain)
					{
						PrevChain->Next = Chain->Next;
						delete Chain;
						Elements--;
					}
				}				
			}

		}

		Set<K>& operator=(const Set<K>& Other)
		{			
			Copy(Other);
			return *this;
		}

		typename SetIterator<K> begin()
		{
			return SetIterator<K>(Table);
		}

		typename SetIterator<K> end()
		{
			return SetIterator<K>();
		}

		int32 GetSize() const
		{
			return Elements;
		}

		void Copy(const Set<K>& Other)
		{
			// Deep copy the map
			for (uint32 i = 0; i < TABLE_SIZE; i++)
			{

				// I know we could call clear instead, but this requires one less pass
				SetChain<K>* Current = Table[i];
				while (Current)
				{
					SetChain<K>* Next = Current->Next;
					delete Current;
					Current = Next;
				}
				
				Table[i] = nullptr;

				SetChain<K>* Start = Other.Table[i];
				SetChain<K>* NewChainHead = nullptr;
				SetChain<K>* NewChain = nullptr;
				SetChain<K>* NewChainPrev = nullptr;

				while (Start)
				{
					NewChain = new SetChain<K>;
					NewChain->Key = Start->Key;
					NewChain->Next = nullptr;

					if (!NewChainHead)
					{
						NewChainHead = NewChain;
					}

					if (NewChainPrev)
					{
						NewChainPrev->Next = NewChain;
					}

					NewChainPrev = NewChain;
					Start = Start->Next;
				}

				Table[i] = NewChainHead;


			}
		}

	private:

		SetChain<K>* Table [TABLE_SIZE];
		int32 Elements;
	};

}
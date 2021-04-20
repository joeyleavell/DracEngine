#pragma once

#include "Algorithm/Algorithm.h"
#include <iostream>

#define TABLE_SIZE 1

namespace Ry
{
	template <class K, class V>
	struct EXPORT_ONLY MapChain
	{
		K key;
		V value;
		MapChain<K, V>* next;

		MapChain():
		key(),
		next(nullptr)
		{
			
		}
	};

	template<class K, class V>
	class EXPORT_ONLY KeyIterator : public Ry::Iterator<K>
	{
	public:

		KeyIterator(const KeyIterator& Other):
		HashTable(Other.HashTable),
		TableIndex(Other.TableIndex),
		CurrentEntry(Other.CurrentEntry),
		NextEntry(Other.NextEntry),
		NextIndex(Other.NextIndex)
		{
		}
		
		KeyIterator(const KeyIterator&& Other) noexcept:
		HashTable(Other.HashTable),
		TableIndex(Other.TableIndex),
		CurrentEntry(Other.CurrentEntry),
		NextEntry(Other.NextEntry),
		NextIndex(Other.NextIndex)
		{
		}

		KeyIterator(MapChain<K, V>** HashTable):
		HashTable(HashTable),
		TableIndex(-1),
		CurrentEntry(nullptr),
		NextEntry(nullptr),
		NextIndex(-1)
		{
			CurrentEntry = FindNext(TableIndex, -1);

			if(TableIndex >= 0)
			{
				NextEntry = FindNext(NextIndex, TableIndex + 1);
			}
			else
			{
				NextEntry = nullptr;
				NextIndex = -1;
			}

		}

		virtual ~KeyIterator() = default;

		explicit operator bool() const override
		{
			return CurrentEntry != nullptr;
		}

		K* operator*() const override
		{
			// Check if we've reached the end of the hash table
			if (TableIndex >= TABLE_SIZE)
				return nullptr;

			return &CurrentEntry->key;
		}

		K* Key() const
		{
			return operator*();
		}

		V* Value() const
		{
			// Check if we've reached the end of the hash table
			if (TableIndex >= TABLE_SIZE)
				return nullptr;

			return &CurrentEntry->value;
		}

		KeyIterator& operator++() override
		{
			// Check if there is another table entry at this index (collision)
			if (CurrentEntry->next)
			{
				CurrentEntry = CurrentEntry->next;
			}
			else
			{
				CurrentEntry = NextEntry;
				TableIndex = NextIndex;

				if(TableIndex >= 0)
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

		KeyIterator<K, V>& operator=(const KeyIterator<K, V>& Other)
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

		KeyIterator<K, V>& operator=(KeyIterator<K, V>&& Other) noexcept
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
		MapChain<K, V>* FindNext(int32& OutIndex, int32 StartPointIndex)
		{
			int32 Index = StartPointIndex + 1;
			
			// Scan for the next table entry
			bool Found = false;
			while (!Found && Index < TABLE_SIZE)
			{
				if (HashTable[Index])
				{
					this->CurrentEntry = HashTable[Index];
					Found = true;
				}
				else
				{
					++Index;
				}

			}

			if(Found)
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
		
		MapChain<K, V>** HashTable;

		// These values can be negative, indicating no next value.
		// The next value is stored as a convenience to allow explicit checking
		int32 TableIndex;
		int32 NextIndex;
		
		MapChain<K, V>* CurrentEntry;
		MapChain<K, V>* NextEntry;

	};

	template <class K, class V>
	class EXPORT_ONLY Map
	{
	public:

		Map()
		{
			for (uint32 i = 0; i < TABLE_SIZE; i++)
				table[i] = nullptr;
		}

		Map(const Map<K, V>& Other)
		{
			for (uint32 i = 0; i < TABLE_SIZE; i++)
				table[i] = nullptr;
			
			Copy(Other);
		}

		KeyIterator<K, V> CreateKeyIterator()
		{
			KeyIterator<K, V> KeyItr (table);
			return KeyItr;
		}

		void Clear()
		{
			for(int32 TableIndex = 0; TableIndex < TABLE_SIZE; TableIndex++)
			{
				MapChain<K, V>* Chain = table[TableIndex];
				while(Chain)
				{
					MapChain<K, V>* Next = Chain->next;

					delete Chain;
					Chain = Next;
				}

				table[TableIndex] = nullptr;
			}
		}

		bool contains(const K& key) const
		{
			uint32 hash_value = Ry::Hash(key);
			uint32 bucket = hash_value % TABLE_SIZE;
			MapChain<K, V>* chain = table[bucket];

			if (chain == nullptr)
				return false;

			while (chain != nullptr && !(chain->key == key))
				chain = chain->next;

			if (chain == nullptr)
				return false;
			else
				return true;
		}

		void insert(const K& key, const V& value)
		{
			uint32 hash_value = Ry::Hash(key);
			uint32 bucket = hash_value % TABLE_SIZE;
			
			// Travel to the last element in the bucket
			// OR overwrite key if inserting duplicate
			MapChain<K, V>* head = table[bucket];
			MapChain<K, V>* head_prev = nullptr;
			
			while (head != nullptr)
			{
				// Found existing element
				// Overwrite key
				if(head->key == key)
				{
					head->value = value;
					return;
				}

				head_prev = head;
				head = head->next;
			}

			MapChain<K, V>* new_chain = new MapChain<K, V>();
			new_chain->key = key;
			new_chain->value = value;
			new_chain->next = nullptr;

			if (head_prev == nullptr)
			{
				table[bucket] = new_chain;
			}
			else
			{
				head_prev->next = new_chain;
			}

		}

		V* get(const K& key) const
		{
			uint32 hash_value = Ry::Hash(key);
			MapChain<K, V>* bucket = table[hash_value % TABLE_SIZE];

			if (bucket == nullptr)
				return nullptr;

			// Go through elements in this bucket until we find a match, or we reach the end.
			while (bucket != nullptr && !(bucket->key == key))
			{
				bucket = bucket->next;
			}

			if (bucket == nullptr)
				return nullptr;

			return &bucket->value;
		}

		void remove(const K& key)
		{
			uint32 hash_value = Ry::Hash(key);
			uint32 bucket = hash_value % TABLE_SIZE;
			MapChain<K, V>* chain = table[bucket];

			if (chain == nullptr)
			{
				std::cerr << "ERROR: tried to delete element that was not in hashmap" << std::endl;
			}
			else if (chain->next == nullptr)
			{
				if (chain->key == key)
				{
					delete table[bucket];
					table[bucket] = nullptr;
				}
				else
					std::cerr << "ERROR: tried to delete element that was not in hashmap" << std::endl;
			}
			else
			{
				MapChain<K, V>* prev_chain = table[bucket];
				chain = chain->next;

				while (chain != nullptr && !(chain->key == key))
				{
					prev_chain = chain;
					chain = chain->next;
				}

				if (chain != nullptr)
				{
					prev_chain->next = chain->next;
					delete chain;
				}
				else
				{
					std::cerr << "ERROR: tried to delete element that was not in hashmap" << std::endl;
				}
			}
		}

		Map<K, V>& operator=(const Map<K, V>& Other)
		{
			Copy(Other);
			return *this;
		}

		void Copy(const Map<K, V>& Other)
		{	
			// Deep copy the map
			for (uint32 i = 0; i < TABLE_SIZE; i++)
			{

				// I know we could call clear instead, but this requires one less pass
				MapChain<K, V>* Current = table[i];
				while(Current)
				{
					MapChain<K, V>* Next = Current->next;
					delete Current;
					Current = Next;
				}
				table[i] = nullptr;

				MapChain<K, V>* Start = Other.table[i];
				MapChain<K, V>* NewChainHead = nullptr;
				MapChain<K, V>* NewChain = nullptr;
				MapChain<K, V>* NewChainPrev = nullptr;

				while (Start)
				{
					NewChain = new MapChain<K, V>;
					NewChain->key = Start->key;
					NewChain->value = Start->value;
					NewChain->next = nullptr;

					if(!NewChainHead)
					{
						NewChainHead = NewChain;
					}

					if (NewChainPrev)
					{
						NewChainPrev->next = NewChain;
					}

					NewChainPrev = NewChain;
					Start = Start->next;
				}

				table[i] = NewChainHead;


			}
		}

	private:

		MapChain<K, V>* table[TABLE_SIZE];
	};

}
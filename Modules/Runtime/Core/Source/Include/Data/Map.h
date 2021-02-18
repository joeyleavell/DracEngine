#pragma once

#include "Algorithm/Algorithm.h"
#include <iostream>

#define TABLE_SIZE 500

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

		KeyIterator(const KeyIterator& Other)
		{
			this->HashTable = Other.HashTable;
			this->TableIndex = Other.TableIndex;
			this->CurrentEntry = Other.CurrentEntry;
		}

		KeyIterator(MapChain<K, V>** HashTable)
		{
			this->HashTable = HashTable;
			this->TableIndex = 0;
			this->CurrentEntry = nullptr;

			// Scan for the first table entry
			bool Found = false;
			while (!Found && TableIndex < TABLE_SIZE)
			{
				if (HashTable[TableIndex])
				{
					this->CurrentEntry = HashTable[TableIndex];
					Found = true;
				}

				++TableIndex;
			}
		}

		virtual ~KeyIterator() = default;

		explicit operator bool() const override
		{
			return TableIndex < TABLE_SIZE;
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
				// Scan for the next table entry
				bool Found = false;
				while (!Found && TableIndex < TABLE_SIZE)
				{
					if (HashTable[TableIndex])
					{
						this->CurrentEntry = HashTable[TableIndex];
						Found = true;
					}
					++TableIndex;
				}
			}

			return *this;
		}

	private:
		MapChain<K, V>** HashTable;

		uint32 TableIndex;
		MapChain<K, V>* CurrentEntry;
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

			MapChain<K, V>* new_chain = new MapChain<K, V>();
			new_chain->key = key;
			new_chain->value = value;
			new_chain->next = nullptr;

			// Travel to the last element in the bucket
			MapChain<K, V>* head = table[bucket];
			while (head != nullptr && head->next != nullptr)
				head = head->next;

			if (head == nullptr)
			{
				table[bucket] = new_chain;
			}
			else
			{
				head->next = new_chain;
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
					chain = chain->next;
				}

				if (chain != nullptr)
				{
					prev_chain->next = nullptr;
					delete chain;
				}
				else
				{
					std::cerr << "ERROR: tried to delete element that was not in hashmap" << std::endl;
				}
			}
		}

	private:

		MapChain<K, V>* table[TABLE_SIZE];
	};

}
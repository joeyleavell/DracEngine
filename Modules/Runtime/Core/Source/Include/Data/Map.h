#pragma once

#include "Algorithm/Algorithm.h"
#include <iostream>

#define TABLE_SIZE 5000
#define LOAD_FACTOR 0.75

constexpr uint32 Probe_Linear(uint32 Hash, uint32 Probe, uint32 N)
{
	return (Hash + Probe) % N;
}

namespace Ry
{

	template<typename T>
	struct OAHashBucket
	{
		T Storage;
		bool bOccupied;

		OAHashBucket()
		{
			bOccupied = false;
		}
	};

	template<typename T>
	class OAHashTableIterator
	{
	public:

		OAHashTableIterator()
		{
			this->IteratingTable = nullptr;
			this->TableSize = 0;
			this->CurrentBucket = -1;
		}
		
		OAHashTableIterator(const OAHashTableIterator& Other)
		{
			Init(Other.IteratingTable, Other.TableSize);
		}

		OAHashTableIterator(OAHashTableIterator&& Other) noexcept
		{
			Init(Other.IteratingTable, Other.TableSize);
		}

		OAHashTableIterator(const OAHashBucket<T>* Table, int32 TableSize)
		{
			Init(Table, TableSize);
		}

		void Init(const OAHashBucket<T>* Table, int32 TableSize)
		{
			this->IteratingTable = Table;
			this->TableSize = TableSize;
			this->CurrentBucket = -1;

			Next();
		}

		const T& GetCurrent()
		{
			return IteratingTable[CurrentBucket].Storage;
		}

		void Next()
		{
			do
			{
				CurrentBucket++;
			} while (CurrentBucket < TableSize && !IteratingTable[CurrentBucket].bOccupied);
		}

		OAHashTableIterator& operator++()
		{
			Next();
			return *this;
		}

		operator bool()
		{
			return HasNext();
		}

		bool HasNext() const
		{
			return CurrentBucket < TableSize;
		}

		T& operator*() const
		{
			return IteratingTable[CurrentBucket].Storage;
		}

		OAHashTableIterator<T>& operator=(OAHashTableIterator<T>&& Other) noexcept
		{
			Init(Other.IteratingTable, Other.TableSize);
			return *this;
		}

		OAHashTableIterator<T>& operator=(const OAHashTableIterator<T>& Other)
		{
			Init(Other.IteratingTable, Other.TableSize);
			return *this;
		}

		bool operator==(const OAHashTableIterator<T>& Other) const
		{
			return IteratingTable == Other.IteratingTable && CurrentBucket == Other.CurrentBucket;
		}

	private:
		const OAHashBucket<T>* IteratingTable;
		int32 CurrentBucket;
		int32 TableSize;
	};

	// Open addressing table using a specified probe funciton
	template<typename T, uint32 ProbeFunc(uint32 HashValue, uint32 Probe, uint32 TableSize)>
	class OAHashTable
	{
	public:

		OAHashTable(int32 TableSize = TABLE_SIZE)
		{
			// Allocate buckets
			this->Table = nullptr;
			this->Elements = 0;
			Rehash(TableSize);
		}

		OAHashTableIterator<T> CreateTableIterator() const
		{
			OAHashTableIterator<T> Res(Table, TableSize);
			return Res;
		}

		template<typename K>
		T& Get(const K& Key)
		{
			uint32 HashValue = Ry::Hash(Key);
			int32 Probe = 0;
			
			while (Probe < TableSize)
			{
				uint32 BucketIndex = ProbeFunc(HashValue, Probe, TableSize);
				OAHashBucket<T>& Bucket = Table[BucketIndex];

				if (Bucket.bOccupied && Bucket.Storage == Key)
				{
					return Bucket.Storage;
				}

				Probe++;
			}

			CORE_ASSERTF(false, "Value not in hash table, undefined behavior");
		}

		void Insert(const T& Value)
		{
			bool bInserted = false;
			
			uint32 HashValue = Ry::Hash(Value);

			int32 Probe = 0;
			while(Probe < TableSize && !bInserted)
			{
				uint32 BucketIndex = ProbeFunc(HashValue, Probe, TableSize);
				OAHashBucket<T>& Bucket = Table[BucketIndex];

				if(!Bucket.bOccupied)
				{
					// Insert value into bucket, set bInserted and bOccupied to true
					Bucket.bOccupied = true;
					Bucket.Storage = Value;
					bInserted = true;
					Elements++;

					// Calc new load factor, rehash if needed
					float LoadFactor = Elements / (float)TableSize;
					if(LoadFactor >= LOAD_FACTOR)
					{
						Rehash(TableSize * 2);
					}
				}
				
				Probe++;
			}

		}

		template<typename K>
		bool Remove(const K& Value)
		{
			uint32 HashValue = Ry::Hash(Value);

			int32 Probe = 0;
			while (Probe < TableSize)
			{
				uint32 BucketIndex = ProbeFunc(HashValue, Probe, TableSize);
				OAHashBucket<T>& Bucket = Table[BucketIndex];

				if(Bucket.bOccupied && Bucket.Storage == Value)
				{					
					Bucket.bOccupied = false;

					if constexpr (std::is_destructible<T>::value)
					{
						Bucket.Storage.~T(); // Manually call destructor
					}

					Elements--;
					
					return true;
				}

				Probe++;
			}

			return false;
		}

		template<typename K>
		bool Contains(const K& Value)
		{
			uint32 HashValue = Ry::Hash(Value);

			int32 Probe = 0;
			while (Probe < TableSize)
			{
				uint32 BucketIndex = ProbeFunc(HashValue, Probe, TableSize);
				OAHashBucket<T>& Bucket = Table[BucketIndex];

				if (Bucket.bOccupied && Bucket.Storage == Value)
				{
					return true;
				}

				Probe++;
			}

			return false;
		}

		void Clear()
		{
			this->Elements = 0;
			
			for (int32 Bucket = 0; Bucket < TableSize; Bucket++)
			{
				if (Table[Bucket].bOccupied)
				{
					if constexpr (std::is_destructible<T>::value)
						Table[Bucket].Storage.~T(); // Manually call destructor

					Table[Bucket].bOccupied = false;
				}
			}
		}

	private:

		void Rehash(int32 NewSize)
		{
			// Cache old table for rehashing
			int32 OldSize = TableSize;
			OAHashBucket<T>* OldTable = Table;

			// Create new table
			this->Table = new OAHashBucket<T>[NewSize];
			this->TableSize = NewSize;

			// Rehash all values from old table
			if(OldTable)
			{
				this->Elements = 0;
				for(int32 Bucket = 0; Bucket < OldSize; Bucket++)
				{
					if(OldTable[Bucket].bOccupied)
					{
						Insert(OldTable[Bucket].Storage);
					}
				}

				// Delete old table
				delete[] OldTable;
			}
		}

		int32 TableSize;
		OAHashBucket<T>* Table;
		int32 Elements;
	};

	template<typename K, typename V>
	struct OAHashContainer
	{
		K Key;
		V Value;

		OAHashContainer()
		{
			
		}

		OAHashContainer(const K& InKey, const V& InValue)
		{
			this->Key = InKey;
			this->Value = InValue;
		}

		bool operator==(const K& InKey) const
		{
			return Key == InKey;
		}
		
	};

	// Hash implementation for openly addressed hash containers
	template <typename K, typename V>
	uint32 Hash(OAHashContainer<K*, V> Object)
	{
		return Hash<K>(Object.Key);
	}

	template <typename K, typename V>
	uint32 Hash(OAHashContainer<K, V> Object)
	{
		return Hash<K>(Object.Key);
	}


	template<typename K, typename V, uint32 ProbeFunc(uint32 HashValue, uint32 Probe, uint32 TableSize) = Probe_Linear>
	class OAPairIterator
	{
	public:

		OAPairIterator(OAHashTable<OAHashContainer<K, V>, ProbeFunc>& InTable)
		{
			this->TableIterator = InTable.CreateTableIterator();
		}

		OAPairIterator<K, V, ProbeFunc>& operator++()
		{
			++TableIterator;
			return *this;
		}

		OAHashContainer<K, V> operator*() const
		{
			return *TableIterator;
		}

		const K& GetKey()
		{
			return TableIterator.GetCurrent().Key;
		}

		const V& GetValue()
		{
			return TableIterator.GetCurrent().Value;
		}


		operator bool() const
		{
			return TableIterator.HasNext();
		}

		bool operator==(const OAPairIterator<K, V, ProbeFunc>& Other) const
		{
			return TableIterator == Other.TableIterator;
		}

	protected:

		OAHashTableIterator<OAHashContainer<K, V>> TableIterator;

	};
	
	template<typename K, typename V, uint32 ProbeFunc(uint32 HashValue, uint32 Probe, uint32 TableSize) = Probe_Linear>
	class OAHashMap
	{
	public:
		friend class OAPairIterator<K, V, ProbeFunc>;

		OAHashMap(int32 TableSize = TABLE_SIZE):
		Table(TableSize)
		{
			
		}

		~OAHashMap()
		{
			
		}

		V& Get(const K& Key)
		{
			return Table.Get(Key).Value;
		}

		bool Contains(const K& Key)
		{
			return Table.Contains(Key);
		}

		bool Remove(const K& Key)
		{
			return Table.Remove(Key);
		}

		void Insert(const K& Key, const V& Value)
		{
			Table.Insert(OAHashContainer<K, V>(Key, Value));
		}

		void Clear()
		{
			Table.Clear();
		}

		OAPairIterator<K, V, ProbeFunc> CreatePairIterator()
		{
			return OAPairIterator<K, V, ProbeFunc>(Table);
		}

	private:

		OAHashTable<OAHashContainer<K, V>, ProbeFunc> Table;
		
	};
	
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

			if (chain->next == nullptr)
			{
				if (chain->key == key)
				{
					delete table[bucket];
					table[bucket] = nullptr;
				}
			}
			else if(chain->key == key)
			{
				table[bucket] = chain->next;
				delete chain;
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
#pragma once

#include "Core/Platform.h"
#include "Algorithm/Algorithm.h"
#include "CoreGen.h"

#define TABLE_SIZE 10
#define LOAD_FACTOR 0.75

namespace Ry
{
	constexpr uint32 Probe_Linear(uint32 Hash, uint32 Probe, uint32 N)
	{
		return (Hash + Probe) % N;
	}

	template<typename T>
	struct OAHashBucket
	{
		T Storage;
		bool bOccupied;
		bool bDeleted;

		OAHashBucket()
		{
			bOccupied = false;

			// Keep track of last element stored in this bucket
			bDeleted = false;
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
			if (!IteratingTable)
				return false;
			
			return CurrentBucket < TableSize;
		}

		const T& operator*() const
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
			if(!HasNext() && !Other.HasNext())
				return true;

			return IteratingTable == Other.IteratingTable && CurrentBucket == Other.CurrentBucket;
		}

	private:
		const OAHashBucket<T>* IteratingTable;
		int32 CurrentBucket;
		int32 TableSize;
	};

	// Open addressing table using a specified probe funciton
	template<typename T, uint32 ProbeFunc(uint32 HashValue, uint32 Probe, uint32 TableSize)>
	class EXPORT_ONLY OAHashTable
	{
	public:

		OAHashTable(int32 TableSize = TABLE_SIZE)
		{
			// Allocate buckets
			this->Table = nullptr;
			this->Elements = 0;
			Rehash(TableSize);
		}

		OAHashTableIterator<T> begin() const
		{
			OAHashTableIterator<T> Res(Table, TableSize);
			return Res;
		}

		OAHashTableIterator<T> end() const
		{
			OAHashTableIterator<T> Res;
			return Res;
		}

		OAHashTableIterator<T> CreateTableIterator() const
		{
			OAHashTableIterator<T> Res(Table, TableSize);
			return Res;
		}

		bool IsEmpty() const
		{
			return Elements <= 0;
		}

		int32 GetSize() const
		{
			return Elements;
		}

		template<typename K>
		T& Get(const K& Key) const
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
			while (Probe < TableSize && !bInserted)
			{
				uint32 BucketIndex = ProbeFunc(HashValue, Probe, TableSize);
				OAHashBucket<T>& Bucket = Table[BucketIndex];

				if (!Bucket.bOccupied)
				{
					// Insert value into bucket, set bInserted and bOccupied to true
					Bucket.bOccupied = true;
					Bucket.Storage = Value;
					
					bInserted = true;
					Elements++;

					// Calc new load factor, rehash if needed
					float LoadFactor = Elements / (float)TableSize;
					if (LoadFactor >= LOAD_FACTOR)
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

				if (Bucket.bOccupied && Bucket.Storage == Value)
				{
					Bucket.bOccupied = false;
					Bucket.bDeleted = true;

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
		bool Contains(const K& Value) const
		{
			uint32 HashValue = Ry::Hash(Value);

			int32 Probe = 0;
			while (Probe < TableSize)
			{
				uint32 BucketIndex = ProbeFunc(HashValue, Probe, TableSize);
				OAHashBucket<T>& Bucket = Table[BucketIndex];

				if (!Bucket.bOccupied && !Bucket.bDeleted)
					return false;

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
			if (OldTable)
			{
				this->Elements = 0;
				for (int32 Bucket = 0; Bucket < OldSize; Bucket++)
				{
					if (OldTable[Bucket].bOccupied)
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

}

#pragma once

#include "PoolAllocator.gen.h"
#include "Allocator.h"
#include "Data/Set.h"
#include "Data/Map.h"

namespace Ry
{

	struct PoolChunk
	{
		void* Allocation;
		PoolChunk* Next;

		PoolChunk(int32 Size)
		{
			this->Next = nullptr;
			this->Allocation = malloc(Size);
		}
	};

	struct PoolBlock
	{
		PoolChunk* Free_Head;
		PoolChunk* Free_Tail;

		Ry::OAHashMap<void*, PoolChunk*> Used;

		PoolBlock(int32 Chunks, int32 ChunkSize)
		{
			Free_Head = nullptr;
			Free_Tail = nullptr;
			
			int32 Chunk = 0;
			while(Chunk < Chunks)
			{
				PoolChunk* New = new PoolChunk(ChunkSize);
				if(!Free_Head)
				{
					Free_Head = New;
					Free_Tail = Free_Head;
				}
				else
				{
					Free_Tail->Next = New;
					Free_Tail = New;
				}

				Chunk++;
			}
		}

		bool HasSpace()
		{
			return Free_Head != nullptr;
		}

		void Return(void* Memory)
		{
			PoolChunk* Chunk = Used.Get(Memory);
			CORE_ASSERT(Chunk);
			Chunk->Next = nullptr;

			Used.Remove(Memory);

			if(Free_Tail == Free_Head)
			{
				if(!Free_Tail)
				{
					// Empty pool	
					Free_Head = Free_Tail = Chunk;
				}
				else
				{
					// Only one element
					Free_Tail = Chunk;
					Free_Head->Next = Chunk;
				}
			}
			else
			{
				// More than one link, append to tail
				Free_Tail->Next = Chunk;				
				Free_Tail = Chunk;
			}

			Allocs--;
		}

		void* Take()
		{
			if (!Free_Head)
				return nullptr;

			PoolChunk* Found = nullptr;

			if (Free_Tail == Free_Head)
			{
				// Only one element
				Found = Free_Head;
				Free_Head = Free_Tail = nullptr;
			}
			else
			{
				// More than one element, grab head
				Found = Free_Head;
				Free_Head = Free_Head->Next;
			}

			Allocs++;

			if(Found)
			{
				Used.Insert(Found->Allocation, Found);
				return Found->Allocation;
			}
			else
			{
				return nullptr;
			}
		}

		int32 Allocs = 0;

	};

	class CORE_MODULE PoolAllocator : public Allocator
	{
	public:

		PoolAllocator(int32 InChunkSize, int32 InChunks)
		{
			this->ChunkCount = InChunks;
			this->ChunkSize = InChunkSize;
			
			// Create one block initially
			PoolBlock* NewBlock = new PoolBlock(ChunkCount, ChunkSize);
			Available.Add(NewBlock);
		}
		
		void* Allocate(int32 Size = 0)
		{
			CORE_ASSERT(Available.GetSize() > 0);

			PoolBlock* Next = Available[0];

			// Take a chunk, assume it's valid
			void* NextChunk = Next->Take();
			CORE_ASSERT(NextChunk);

			if(!Next->HasSpace())
			{
				// Clear the current pool from available
				Available.RemoveAt(0);
				Filled.Insert(Next);

				if(Available.IsEmpty())
				{
					// Allocate another block, this one just became empty
					PoolBlock* NewBlock = new PoolBlock(ChunkCount, ChunkSize);
					Available.Add(NewBlock);
				}
			}

			// Track this chunk in a mapping for fast lookup later
			Used.Insert(NextChunk, Next);

			Allocs++;

			return NextChunk;
		}
		
		void Free(void* Chunk)
		{
			PoolBlock* Block = Used.Get(Chunk);

			// Assume this block is valid i.e. we're not returning memory we didn't borrow
			CORE_ASSERT(Block);

			Block->Return(Chunk);
			Used.Remove(Chunk);

			// If the block was previously empty, return it to the available pool
			if(Filled.Contains(Block))
			{
				Filled.Remove(Block);
				Available.Add(Block);
			}

			Allocs--;
		}

	private:

		int32 Allocs = 0;

		int32 ChunkCount;
		int32 ChunkSize;		

		Ry::ArrayList<PoolBlock*> Available;
		Ry::Set<PoolBlock*> Filled;
		Ry::OAHashMap<void*, PoolBlock*> Used;

	};
	
}


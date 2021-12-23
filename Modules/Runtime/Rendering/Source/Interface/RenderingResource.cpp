#include "Interface/RenderingResource.h"
#include "Core/Globals.h"

namespace Ry
{

	namespace Std140Helpers
	{
		int32 SizeOf(const ShaderPrimitiveDataType& PrimData)
		{
			int32 FloatN = sizeof(float);

			switch (PrimData)
			{

				// 1
			case ShaderPrimitiveDataType::Float:
				return FloatN;

				// 2
			case ShaderPrimitiveDataType::Float2:
			case ShaderPrimitiveDataType::Float1x2:
			case ShaderPrimitiveDataType::Float2x1:
				return FloatN * 2;

				// 3
			case ShaderPrimitiveDataType::Float3:
			case ShaderPrimitiveDataType::Float1x3:
			case ShaderPrimitiveDataType::Float3x1:
				return FloatN * 3;

				// 4
			case ShaderPrimitiveDataType::Float4:
			case ShaderPrimitiveDataType::Float4x1:
			case ShaderPrimitiveDataType::Float1x4:
			case ShaderPrimitiveDataType::Float2x2:
				return FloatN * 4;

				// 6
			case ShaderPrimitiveDataType::Float3x2:
			case ShaderPrimitiveDataType::Float2x3:
				return FloatN * 6;

				// 8
			case ShaderPrimitiveDataType::Float4x2:
			case ShaderPrimitiveDataType::Float2x4:
				return FloatN * 8;

				// 9
			case ShaderPrimitiveDataType::Float3x3:
				return FloatN * 9;

				// 12
			case ShaderPrimitiveDataType::Float4x3:
			case ShaderPrimitiveDataType::Float3x4:
				return FloatN * 12;

				// 16
			case ShaderPrimitiveDataType::Float4x4:
				return FloatN * 16;

				// Unknown
			default:
				Ry::Log->LogError("Unknown element when calculating Vulkan size, uniforms probably won't work");
				return 0;
			}
		}

		int32 CalcBufferSize(const ConstantBuffer* Buffer)
		{
			int32 Offset = 0;

			for (const BufferMember* Member : Buffer->BufferMembers)
			{
				CalcMemberSize(Member, Offset);
			}

			return Offset;
		}

		void CalcMemberSize(const BufferMember* Member, int32& CurrentSize)
		{
			int32 Alignment = CalcAlignmentRequirement(Member);

			for (int32 Index = 0; Index < Member->ArraySize; Index++)
			{
				// Correct the alignment if needed
				if (CurrentSize % Alignment != 0)
				{
					CurrentSize = (static_cast<int32>(CurrentSize / Alignment) + 1) * Alignment;
				}

				// Check if this is a primitive or a struct
				const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Member);
				const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Member);

				if (Prim)
				{
					// todo: add support for arrays
					int32 DataSize = SizeOf(Prim->DataType) * Prim->ArraySize;
					CurrentSize += DataSize;
				}
				else if (Struct)
				{
					for (const BufferMember* Child : Struct->StructMembers)
					{
						CalcMemberSize(Child, CurrentSize);
					}
				}
			}
		}

		int32 CalcAlignmentRequirement(const BufferMember* Root)
		{
			const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Root);
			const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Root);

			if (Prim)
			{
				return CalcAlignmentRequirement(Prim->DataType);
			}
			else if (Struct)
			{
				// Alignment is equal to the largest alignment among the children members

				int32 LargestAlignment = 0;

				for (const BufferMember* ChildMember : Struct->StructMembers)
				{
					int32 Alignment = CalcAlignmentRequirement(ChildMember);
					if (Alignment > LargestAlignment)
					{
						LargestAlignment = Alignment;
					}
				}

				return LargestAlignment;
			}

			// This is an error
			CORE_ASSERT(false);

			return -1;
		}

		int32 CalcAlignmentRequirement(ShaderPrimitiveDataType Primitive)
		{
			// This will work independent of if the primitive data type is an array or not

			int32 FloatN = sizeof(float);

			// Taken from https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources
			switch (Primitive)
			{

				/* Arrays have the alignment of their primitive type */
			case ShaderPrimitiveDataType::Float:
				return FloatN;

				/* All 2 column matrix types get 2N alignment */
			case ShaderPrimitiveDataType::Float1x2:
			case ShaderPrimitiveDataType::Float2x2:
			case ShaderPrimitiveDataType::Float3x2:
			case ShaderPrimitiveDataType::Float4x2:
			case ShaderPrimitiveDataType::Float2:
				return 2 * FloatN;
				break;

				/* 3 and 4 column matrices get same alignment as 3/4 element vectors */
			case ShaderPrimitiveDataType::Float1x3:
			case ShaderPrimitiveDataType::Float2x3:
			case ShaderPrimitiveDataType::Float3x3:
			case ShaderPrimitiveDataType::Float4x3:
			case ShaderPrimitiveDataType::Float1x4:
			case ShaderPrimitiveDataType::Float2x4:
			case ShaderPrimitiveDataType::Float3x4:
			case ShaderPrimitiveDataType::Float4x4:
			case ShaderPrimitiveDataType::Float3:
			case ShaderPrimitiveDataType::Float4:
				return 4 * FloatN;

			default:
				Ry::Log->LogError("Unknown element when calculating Vulkan alignment, uniforms probably won't work");
				return 0;
			}

		}

		void PrintTabs(int32 Tabs)
		{
			for (int32 Tab = 0; Tab < Tabs; Tab++)
			{
				std::cout << "\t";
			}
		}

		void PrintData_Helper(const BufferMember* Member, int32 TabCount, int32& Offset)
		{
			const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Member);
			const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Member);

			PrintTabs(TabCount);
			if (Prim)
			{

				int32 StartOffset = Offset;
				int32 Alignment = CalcAlignmentRequirement(Member);
				// Do alignments for array indicies
				for (int Index = 0; Index < Member->ArraySize; Index++)
				{
					if (Offset % Alignment != 0)
					{
						Offset = (static_cast<int32>(Offset / Alignment) + 1) * Alignment;
					}
					Offset += SizeOf(Prim->DataType);
				}
				int32 EndOffset = Offset;

				std::cout << *ToString(Prim->DataType) << " " << *Prim->Id;

				if (Prim->ArraySize > 1)
				{
					std::cout << "[" << Prim->ArraySize << "]";
				}

				std::cout << "; offset=[" << StartOffset << ", " << EndOffset << "] alignment=" << Alignment << std::endl;
			}
			else if (Struct)
			{
				int32 StructSize = 0;
				CalcMemberSize(Struct, StructSize);

				int32 Alignment = CalcAlignmentRequirement(Member);
				if (Offset % Alignment != 0)
				{
					Offset = (static_cast<int32>(Offset / Alignment) + 1) * Alignment;
				}
				int32 StartOffset = Offset;
				int32 EndOffset = StartOffset + Member->ArraySize * StructSize;

				PrintTabs(TabCount);
				std::cout << "struct " << *Prim->Id;
				if (Prim->ArraySize > 1)
				{
					PrintTabs(TabCount);
					std::cout << "[" << Prim->ArraySize << "]";
				}

				std::cout << " offset = [" << StartOffset << ", " << EndOffset << "] alignment = " << Alignment << std::endl;

				PrintTabs(TabCount);
				std::cout << "\n{" << std::endl;
				for (const BufferMember* ChildMember : Struct->StructMembers)
				{
					PrintData_Helper(ChildMember, TabCount + 1, Offset);
				}
				PrintTabs(TabCount);
				std::cout << "\n}" << std::endl;

			}

		}

		void PrintData(const Ry::ArrayList<ConstantBuffer*> ConstantBuffers)
		{
			// Print data for all constant buffers
			for (const ConstantBuffer* ConstBuffer : ConstantBuffers)
			{
				for (const BufferMember* Member : ConstBuffer->BufferMembers)
				{
					int32 Offset = 0;
					PrintData_Helper(Member, 0, Offset);
				}
			}
		}

		void MapData_Helper(OAHashMap<Ry::String, BufferRef>& OutRefs, const BufferMember* Member, Ry::String ParentId, int32& Marker)
		{
			int32 Alignment = CalcAlignmentRequirement(Member);

			for (int32 ChildIndex = 0; ChildIndex < Member->ArraySize; ChildIndex++)
			{
				// Do alignment correction here
				if (Marker % Alignment != 0)
				{
					Marker = (static_cast<int32>(Marker / Alignment) + 1) * Alignment;
				}

				Ry::String DataId;

				// Determine data ID based on array status
				if (Member->ArraySize == 1)
				{
					DataId = Member->Id;
				}
				else
				{
					DataId = Member->Id + "[" + Ry::to_string(ChildIndex) + "]";
				}

				Ry::String FullId = ParentId.IsEmpty() ? DataId : ParentId + "." + DataId;

				BufferRef URef;
				URef.Offset = Marker;

				const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Member);
				const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Member);

				if (Prim)
				{
					URef.bIsPrimitive = true;
					URef.SlotSize = SizeOf(Prim->DataType);

					// Now add the size of the primitive. Primitives are the only things that contribute to the actual size of the buffer, structs are just composite
					Marker += URef.SlotSize;
				}
				else if (Struct)
				{
					URef.bIsPrimitive = false;

					for (const BufferMember* ChildMember : Struct->StructMembers)
					{
						// Map the data for the child member
						MapData_Helper(OutRefs, ChildMember, FullId, Marker);
					}
				}

				OutRefs.Insert(FullId, URef);
			}

		}

		void MapData(OAHashMap<Ry::String, BufferRef>& OutRefs, const ConstantBuffer* Src)
		{
			Ry::String ParentID = "";
			int32 Marker = 0;

			for (const BufferMember* Member : Src->BufferMembers)
			{
				MapData_Helper(OutRefs, Member, ParentID, Marker);
			}
		}
	}

	
}
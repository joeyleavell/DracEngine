#include "GLResources.h"
#include "Core/Globals.h"
#include "GLTexture.h"

namespace Ry
{
	int32 GLSizeOf(const ShaderPrimitiveDataType& PrimData)
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

	int32 GLCalcBufferSize(const ConstantBuffer* Buffer)
	{
		int32 Offset = 0;

		for (const BufferMember* Member : Buffer->BufferMembers)
		{
			GLCalcMemberSize(Member, Offset);
		}

		return Offset;
	}

	void GLCalcMemberSize(const BufferMember* Member, int32& CurrentSize)
	{
		int32 Alignment = GLCalcAlignmentRequirement(Member);

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
				int32 DataSize = GLSizeOf(Prim->DataType) * Prim->ArraySize;
				CurrentSize += DataSize;
			}
			else if (Struct)
			{
				for (const BufferMember* Child : Struct->StructMembers)
				{
					GLCalcMemberSize(Child, CurrentSize);
				}
			}
		}
	}

	int32 GLCalcAlignmentRequirement(const BufferMember* Root)
	{
		const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Root);
		const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Root);

		if (Prim)
		{
			int32 PrimAlign = GLCalcAlignmentRequirement(Prim->DataType);

			// Round up to 4N if an array
			if(PrimAlign < 4 * sizeof(float) && Root->ArraySize > 1)
			{
				PrimAlign = 4 * sizeof(float);
			}
			
			return PrimAlign;
		}
		else if (Struct)
		{
			// Alignment is equal to the largest alignment among the children members

			int32 LargestAlignment = 0;

			for (const BufferMember* ChildMember : Struct->StructMembers)
			{
				int32 Alignment = GLCalcAlignmentRequirement(ChildMember);
				if (Alignment > LargestAlignment)
				{
					LargestAlignment = Alignment;
				}
			}

			return LargestAlignment;
		}

	}

	int32 GLCalcAlignmentRequirement(ShaderPrimitiveDataType Primitive)
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

	GLResources::GLResources(const ResourceLayout* CreateInfo, SwapChain* SC):
	ResourceSet(CreateInfo, SC)
	{
		for (ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			// Create new mapped uniform buffer
			MappedUniformBuffer* NewBuff = new MappedUniformBuffer;
			NewBuff->ConstBuffer = ConstBuffer;
			NewBuff->HostBufferSize = GLCalcBufferSize(ConstBuffer);
			NewBuff->HostDataBuffer = new uint8[NewBuff->HostBufferSize];

			glGenBuffers(1, &NewBuff->Ubo);

			// Give initial buffer data
			glBindBuffer(GL_UNIFORM_BUFFER, NewBuff->Ubo);
			glBufferData(GL_UNIFORM_BUFFER, NewBuff->HostBufferSize, nullptr, GL_DYNAMIC_DRAW); // todo: change this?
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			MapData(NewBuff, ConstBuffer);
			MappedUniformBuffers.insert(ConstBuffer->Name, NewBuff);
		}
	}

	void GLResources::CreateBuffer()
	{

	}

	void GLResources::DeleteBuffer()
	{
		for (ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			MappedUniformBuffer* Buff = *MappedUniformBuffers.get(ConstBuffer->Name);

			glDeleteBuffers(1, &Buff->Ubo);

			delete[] Buff->HostDataBuffer;

			delete Buff;
		}
	}

	bool GLResources::Update()
	{
		return false;
	}

	void GLResources::FlushBuffer(int32 Frame)
	{
		// Send data
		
		for(const ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			MappedUniformBuffer* Buff = *MappedUniformBuffers.get(ConstBuffer->Name);

			glBindBuffer(GL_UNIFORM_BUFFER, Buff->Ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, Buff->HostBufferSize, Buff->HostDataBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferBase(GL_UNIFORM_BUFFER, ConstBuffer->Index, Buff->Ubo);
		}
	}

	void GLResources::BindTexture(Ry::String TextureName, const Ry::Texture* Resource)
	{
		MappedTexture* NewMappedTexture = nullptr;

		if(MappedTextures.contains(TextureName))
		{
			NewMappedTexture = *MappedTextures.get(TextureName);
		}
		else
		{
			NewMappedTexture = new MappedTexture;
			MappedTextures.insert(TextureName, NewMappedTexture);
			NewMappedTexture->Slot = Textures;

			Textures++;
		}

		if(NewMappedTexture)
		{
			const GLTexture* GLTex = dynamic_cast<const GLTexture*>(Resource);
			
			NewMappedTexture->Texture = GLTex->GetHandle();
			NewMappedTexture->Target = GLTex->GetTarget();
		}

	}

	void GLResources::SetConstant(Ry::String BufferName, Ry::String Id, const void* Data)
	{
		if (!MappedUniformBuffers.contains(BufferName))
		{
			Ry::Log->LogErrorf("Uniform buffer named %s does not exist", *BufferName);
			return;
		}

		MappedUniformBuffer* Buff = *MappedUniformBuffers.get(BufferName);

		if (!Buff->UniformRefs.contains(Id))
		{
			Ry::Log->LogErrorf("Uniform %s in constant buffer %s does not exist", *Id, *BufferName);
			return;
		}

		const UniformRef& Ref = *Buff->UniformRefs.get(Id);
		MemCpy(Buff->HostDataBuffer + Ref.Offset, Ref.SlotSize, Data, Ref.SlotSize);
	}

	void GLResources::MapData(MappedUniformBuffer* Dst, const ConstantBuffer* Src)
	{
		Ry::String ParentID = "";
		int32 Marker = 0;

		for (const BufferMember* Member : Src->BufferMembers)
		{
			MapData_Helper(Dst, Member, ParentID, Marker);
		}
	}

	void GLResources::MapData_Helper(MappedUniformBuffer* Dst, const BufferMember* Member, Ry::String ParentId, int32& Marker)
	{
		int32 Alignment = GLCalcAlignmentRequirement(Member);

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

			UniformRef URef;
			URef.Offset = Marker;

			const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Member);
			const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Member);

			if (Prim)
			{
				URef.bIsPrimitive = true;
				URef.SlotSize = GLSizeOf(Prim->DataType);

				// Now add the size of the primitive. Primitives are the only things that contribute to the actual size of the buffer, structs are just composite
				Marker += URef.SlotSize;
			}
			else if (Struct)
			{
				URef.bIsPrimitive = false;

				for (const BufferMember* ChildMember : Struct->StructMembers)
				{
					// Map the data for the child member
					MapData_Helper(Dst, ChildMember, FullId, Marker);
				}
			}

			Dst->UniformRefs.insert(FullId, URef);
		}

	}
}

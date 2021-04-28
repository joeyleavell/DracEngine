#include "Language/ShaderReflector.h"
#include "Language/spirv_reflect.h"
#include "Language/ShaderCompiler.h"
#include "2D/Batch/Batch.h"
#include "Interface2/RenderAPI.h"
#include "Interface2/RenderingResource.h"

namespace Ry
{

	bool CheckSupport(SpvReflectBlockVariable& Block)
	{
		if (Block.array.dims_count > 1)
		{
			Ry::Log->LogErrorf("Only shader arrays with 1 dimension are supported");
			return false;
		}

		return true;
	}

	PrimitiveBufferMember* ReflectPrimitiveMember(SpvReflectBlockVariable& Block)
	{
		CORE_ASSERT(CheckSupport(Block));

		SpvOp Type = Block.type_description->op;
		Ry::String Name = Block.name;
		uint32 ArraySize = Block.array.dims_count > 0 ? Block.array.dims[0] : 1;

		ShaderPrimitiveDataType DT = ShaderPrimitiveDataType::None;

		if(Type == SpvOpTypeFloat)
		{
			DT = ShaderPrimitiveDataType::Float;
		}

		if (Type == SpvOpTypeVector)
		{
			if(Block.numeric.vector.component_count == 1)
			{
				DT = ShaderPrimitiveDataType::Float;
			}
			if (Block.numeric.vector.component_count == 2)
			{
				DT = ShaderPrimitiveDataType::Float2;
			}
			if (Block.numeric.vector.component_count == 3)
			{
				DT = ShaderPrimitiveDataType::Float3;
			}
			if (Block.numeric.vector.component_count == 4)
			{
				DT = ShaderPrimitiveDataType::Float4;
			}
		}

		if (Type == SpvOpTypeMatrix)
		{
			ShaderPrimitiveDataType Mapped[] = {
				ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x3, ShaderPrimitiveDataType::Float1x4,
				ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x3, ShaderPrimitiveDataType::Float2x4,
				ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x3, ShaderPrimitiveDataType::Float3x4,
				ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x3, ShaderPrimitiveDataType::Float4x4,
			};

			int Index = (Block.numeric.matrix.row_count - 1) * 4 + (Block.numeric.matrix.column_count - 1);

			if(Index < 16)
			{
				DT = Mapped[Index];
			}
			else
			{
				Ry::Log->LogErrorf("Unexpected matrix dims: %d, %d", Block.numeric.matrix.row_count, Block.numeric.matrix.column_count);
			}
		}

		if (DT != ShaderPrimitiveDataType::None)
		{
			return new PrimitiveBufferMember(Name, ShaderPrimitiveDataType::Float, ArraySize);
		}
		else
		{
			return nullptr;
		}

	}

	StructBufferMember* ReflectStructMember(SpvReflectBlockVariable& Block)
	{
		CORE_ASSERT(CheckSupport(Block));
		CORE_ASSERT(Block.type_description->op == SpvOpTypeStruct);
		
		SpvOp Type = Block.type_description->op;
		Ry::String Name = Block.name;
		uint32 ArraySize = Block.array.dims_count > 0 ? Block.array.dims[0] : 1;

		StructBufferMember* Dst = new StructBufferMember;
		Dst->ArraySize = ArraySize;
		Dst->Id = Name;

		// Reflect each member
		int32 MemberCount = Block.member_count;
		for (int32 MemberIndex = 0; MemberIndex < MemberCount; MemberIndex++)
		{
			SpvReflectBlockVariable& RefMember = Block.members[MemberIndex];

			BufferMember* Member = nullptr;

			// Determine if struct or primitive
			if (Block.type_description->op == SpvOpTypeStruct)
			{
				Member = ReflectStructMember(RefMember);
			}
			else
			{
				// Assume primitive (todo: do extra checks to ensure primitive?)
				Member = ReflectPrimitiveMember(RefMember);
			}

			if (Member)
			{
				Dst->StructMembers.Add(Member);
			}
			else
			{
				// Error
				Ry::Log->LogErrorf("Failed to reflect struct member %s", RefMember.name);
			}
		}

		return Dst;
	}

	ConstantBuffer* ReflectConstantBuffer(SpvReflectBlockVariable& Block, int32 Binding, Ry::String Name)
	{
		// Assert that the base type is a struct
		CORE_ASSERT(Binding.block.type_description->op == SpvOpTypeStruct);

		ConstantBuffer* NewBuffer = new ConstantBuffer(Binding, Name);

		// Reflect each member
		int32 MemberCount = Block.member_count;
		for (int32 MemberIndex = 0; MemberIndex < MemberCount; MemberIndex++)
		{
			SpvReflectBlockVariable& RefMember = Block.members[MemberIndex];
			CORE_ASSERT(CheckSupport(RefMember));

			BufferMember* Member = nullptr;

			// Determine if struct or primitive
			if(RefMember.type_description->op == SpvOpTypeStruct)
			{
				Member = ReflectStructMember(RefMember);
			}
			else
			{
				// Assume primitive (todo: do extra checks to ensure primitive?)
				Member = ReflectPrimitiveMember(RefMember);
			}

			if(Member)
			{
				NewBuffer->BufferMembers.Add(Member);
			}
			else
			{
				// Error
				Ry::Log->LogErrorf("Failed to reflect member %s", RefMember.name);
			}
		}

		return NewBuffer;
	}
	
	void ReflectShader(Ry::String ShaderLoc, Ry::ShaderStage Stage, Ry::ArrayList<ResourceSetDescription*>& OutDesc)
	{
		uint8* OutSpirV;
		int32 SpirVSize;
		Ry::String ErrWarn;
		// Compile shader to spirv (this means we should always deploy spirv since we rely on it for reflection)
		if(!CompileToSpirV(ShaderLoc, OutSpirV, SpirVSize, ErrWarn, Stage))
		{
			Ry::Log->LogErrorf("Failed to generate reflection data for shader %s because compilation to spirv failed: \n%s", *ShaderLoc, *ErrWarn);
			return;
		}

		SpvReflectShaderModule Module;
		SpvReflectResult Result = spvReflectCreateShaderModule(SpirVSize, OutSpirV, &Module);
		CORE_ASSERT(Result == SPV_REFLECT_RESULT_SUCCESS);

		uint32 SetCount = 0;
		SpvReflectDescriptorSet** Sets = nullptr;		
		Result = spvReflectEnumerateDescriptorSets(&Module, &SetCount, nullptr);
		CORE_ASSERT(Result == SPV_REFLECT_RESULT_SUCCESS);

		if(SetCount <= 0)
		{
			delete[] Sets;
			return;
		}
		
		Sets = new SpvReflectDescriptorSet*[SetCount];
		Result = spvReflectEnumerateDescriptorSets(&Module, &SetCount, Sets);
		CORE_ASSERT(Result == SPV_REFLECT_RESULT_SUCCESS);

		//Ry::ArrayList<Ry::ResourceSetDescription*> ReflectedDescriptions;

		for(int32 SetIndex = 0; SetIndex < SetCount; SetIndex++)
		{
			SpvReflectDescriptorSet* Set = Sets[SetIndex];

			// Create new resource set description
			Ry::ResourceSetDescription* NewDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Stage }, Set->set);
			
			// Generate bindings for the set
			for(int32 BindingIndex = 0; BindingIndex < Set->binding_count; BindingIndex++)
			{
				SpvReflectDescriptorBinding& Binding = (*Set->bindings)[BindingIndex];

				int32 Index = Binding.binding;
				Ry::String Name = Binding.name;

				if(Binding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
					// Reflect the constant buffer
					ConstantBuffer* Reflect = ReflectConstantBuffer(Binding.block, Index, Name);

					// Add the constant buffer to the resource set description
					NewDesc->AddConstantBuffer(Reflect);
				}
				else if (Binding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || Binding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				{
					// Subtrace texture offset
					TextureBinding* TexBinding = new TextureBinding(Index - T_BUFFER_OFFSET, Name);
					NewDesc->AddTextureBinding(TexBinding);
				}


			}

			NewDesc->CreateDescription();

			OutDesc.Add(NewDesc);
		}

		spvReflectDestroyShaderModule(&Module);

		delete[] OutSpirV;
		delete[] Sets;
	}
	
}

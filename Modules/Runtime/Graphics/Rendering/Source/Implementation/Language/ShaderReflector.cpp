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

	bool TryReflectTypeDesc(SpvReflectTypeDescription* Desc, ShaderVariable& OutVar)
	{
		SpvReflectTypeFlags TypeFlags = Desc->type_flags;
		OutVar.ArraySize = Desc->traits.array.dims_count > 0 ? Desc->traits.array.dims[0] : 1;

		if (TypeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT)
		{
			OutVar.Type = ShaderPrimitiveDataType::Float;
		}

		if (TypeFlags & SPV_REFLECT_TYPE_FLAG_VECTOR)
		{
			if (Desc->traits.numeric.vector.component_count == 1)
			{
				OutVar.Type = ShaderPrimitiveDataType::Float;
			}
			if (Desc->traits.numeric.vector.component_count == 2)
			{
				OutVar.Type = ShaderPrimitiveDataType::Float2;
			}
			if (Desc->traits.numeric.vector.component_count == 3)
			{
				OutVar.Type = ShaderPrimitiveDataType::Float3;
			}
			if (Desc->traits.numeric.vector.component_count == 4)
			{
				OutVar.Type = ShaderPrimitiveDataType::Float4;
			}
		}

		if (TypeFlags & SPV_REFLECT_TYPE_FLAG_MATRIX)
		{
			ShaderPrimitiveDataType Mapped[] = {
				ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x3, ShaderPrimitiveDataType::Float1x4,
				ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x3, ShaderPrimitiveDataType::Float2x4,
				ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x3, ShaderPrimitiveDataType::Float3x4,
				ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x3, ShaderPrimitiveDataType::Float4x4,
			};

			int Index = (Desc->traits.numeric.matrix.row_count - 1) * 4 + (Desc->traits.numeric.matrix.column_count - 1);

			if (Index < 16)
			{
				OutVar.Type = Mapped[Index];
			}
			else
			{
				Ry::Log->LogErrorf("Unexpected matrix dims: %d, %d", Desc->traits.numeric.matrix.row_count, Desc->traits.numeric.matrix.column_count);
			}
		}

		if (OutVar.Type != ShaderPrimitiveDataType::None)
		{
			
			return true;
		}
		else
		{
			return false;
		}
	}


	// bool TryReflectInputVar(SpvReflectInterfaceVariable& Input, ShaderInputVariable& OutVar)
	// {
	// 	CORE_ASSERT(CheckSupport(Block));
	//
	// 	SpvOp Type = Input.type_description->op;
	// 	Ry::String Name;
	//
	// 	if(Input.name)
	// 	{
	// 		Name = Input.name;
	// 	}
	// 	
	// 	uint32 ArraySize = Input.array.dims_count > 0 ? Input.array.dims[0] : 1;
	//
	// 	ShaderPrimitiveDataType DT = ShaderPrimitiveDataType::None;
	//
	// 	if (Type == SpvOpTypeFloat)
	// 	{
	// 		DT = ShaderPrimitiveDataType::Float;
	// 	}
	//
	// 	if (Type == SpvOpTypeVector)
	// 	{
	// 		if (Input.numeric.vector.component_count == 1)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float;
	// 		}
	// 		if (Input.numeric.vector.component_count == 2)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float2;
	// 		}
	// 		if (Input.numeric.vector.component_count == 3)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float3;
	// 		}
	// 		if (Input.numeric.vector.component_count == 4)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float4;
	// 		}
	// 	}
	//
	// 	if (Type == SpvOpTypeMatrix)
	// 	{
	// 		ShaderPrimitiveDataType Mapped[] = {
	// 			ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x3, ShaderPrimitiveDataType::Float1x4,
	// 			ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x3, ShaderPrimitiveDataType::Float2x4,
	// 			ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x3, ShaderPrimitiveDataType::Float3x4,
	// 			ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x3, ShaderPrimitiveDataType::Float4x4,
	// 		};
	//
	// 		int Index = (Input.numeric.matrix.row_count - 1) * 4 + (Input.numeric.matrix.column_count - 1);
	//
	// 		if (Index < 16)
	// 		{
	// 			DT = Mapped[Index];
	// 		}
	// 		else
	// 		{
	// 			Ry::Log->LogErrorf("Unexpected matrix dims: %d, %d", Input.numeric.matrix.row_count, Input.numeric.matrix.column_count);
	// 		}
	// 	}
	//
	// 	if (DT != ShaderPrimitiveDataType::None)
	// 	{
	// 		OutVar = ShaderInputVariable{Name, DT};
	// 		return true;
	// 	}
	// 	else
	// 	{
	// 		return false;
	// 	}
	//
	// }

	// PrimitiveBufferMember* ReflectPrimitiveMember(SpvReflectBlockVariable& Block)
	// {
	// 	CORE_ASSERT(CheckSupport(Block));
	// 	/*SPV_REFLECT_TYPE_FLAG_UNDEFINED = 0x00000000,
	// 		SPV_REFLECT_TYPE_FLAG_VOID = 0x00000001,
	// 		SPV_REFLECT_TYPE_FLAG_BOOL = 0x00000002,
	// 		SPV_REFLECT_TYPE_FLAG_INT = 0x00000004,
	// 		SPV_REFLECT_TYPE_FLAG_FLOAT = 0x00000008,
	// 		SPV_REFLECT_TYPE_FLAG_VECTOR = 0x00000100,
	// 		SPV_REFLECT_TYPE_FLAG_MATRIX = 0x00000200,
	// 		SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE = 0x00010000,
	// 		SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER = 0x00020000,
	// 		SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE = 0x00040000,
	// 		SPV_REFLECT_TYPE_FLAG_EXTERNAL_BLOCK = 0x00080000,
	// 		SPV_REFLECT_TYPE_FLAG_EXTERNAL_ACCELERATION_STRUCTURE = 0x00100000,
	// 		SPV_REFLECT_TYPE_FLAG_EXTERNAL_MASK = 0x00FF0000,
	// 		SPV_REFLECT_TYPE_FLAG_STRUCT = 0x10000000,
	// 		SPV_REFLECT_TYPE_FLAG_ARRAY = 0x20000000,
	// } SpvReflectTypeFlagBits;
	// */
	// 	SpvReflectTypeFlags TypeFlags = Block.type_description->type_flags;
	// 	Ry::String Name = Block.name;
	// 	uint32 ArraySize = Block.array.dims_count > 0 ? Block.array.dims[0] : 1;
	//
	// 	ShaderPrimitiveDataType DT = ShaderPrimitiveDataType::None;
	//
	// 	if(TypeFlags & SPV_REFLECT_TYPE_FLAG_FLOAT)
	// 	{
	// 		DT = ShaderPrimitiveDataType::Float;
	// 	}
	//
	// 	if (TypeFlags & SPV_REFLECT_TYPE_FLAG_VECTOR)
	// 	{
	// 		if(Block.numeric.vector.component_count == 1)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float;
	// 		}
	// 		if (Block.numeric.vector.component_count == 2)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float2;
	// 		}
	// 		if (Block.numeric.vector.component_count == 3)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float3;
	// 		}
	// 		if (Block.numeric.vector.component_count == 4)
	// 		{
	// 			DT = ShaderPrimitiveDataType::Float4;
	// 		}
	// 	}
	//
	// 	if (TypeFlags & SPV_REFLECT_TYPE_FLAG_MATRIX)
	// 	{
	// 		ShaderPrimitiveDataType Mapped[] = {
	// 			ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x2, ShaderPrimitiveDataType::Float1x3, ShaderPrimitiveDataType::Float1x4,
	// 			ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x2, ShaderPrimitiveDataType::Float2x3, ShaderPrimitiveDataType::Float2x4,
	// 			ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x2, ShaderPrimitiveDataType::Float3x3, ShaderPrimitiveDataType::Float3x4,
	// 			ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x2, ShaderPrimitiveDataType::Float4x3, ShaderPrimitiveDataType::Float4x4,
	// 		};
	//
	// 		int Index = (Block.numeric.matrix.row_count - 1) * 4 + (Block.numeric.matrix.column_count - 1);
	//
	// 		if(Index < 16)
	// 		{
	// 			DT = Mapped[Index];
	// 		}
	// 		else
	// 		{
	// 			Ry::Log->LogErrorf("Unexpected matrix dims: %d, %d", Block.numeric.matrix.row_count, Block.numeric.matrix.column_count);
	// 		}
	// 	}
	//
	// 	if (DT != ShaderPrimitiveDataType::None)
	// 	{
	// 		return new PrimitiveBufferMember(Name, DT, ArraySize);
	// 	}
	// 	else
	// 	{
	// 		return nullptr;
	// 	}
	//
	// }

	StructBufferMember* ReflectStructMember(SpvReflectBlockVariable& Block)
	{
		CORE_ASSERT(CheckSupport(Block));
		CORE_ASSERT(Block.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT);
		
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
				ShaderVariable Var;

				if(TryReflectTypeDesc(RefMember.type_description, Var))
				{
					Member = new PrimitiveBufferMember(RefMember.name, Var.Type, Var.ArraySize);
				}
				
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
		CORE_ASSERT(Block.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT);

		ConstantBuffer* NewBuffer = new ConstantBuffer(Binding, Name);

		// Reflect each member
		int32 MemberCount = Block.member_count;
		for (int32 MemberIndex = 0; MemberIndex < MemberCount; MemberIndex++)
		{
			SpvReflectBlockVariable& RefMember = Block.members[MemberIndex];
			CORE_ASSERT(CheckSupport(RefMember));

			BufferMember* Member = nullptr;

			// Determine if struct or primitive
			if(RefMember.type_description->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT)
			{
				Member = ReflectStructMember(RefMember);
			}
			else
			{
				// Assume primitive (todo: do extra checks to ensure primitive?)
				ShaderVariable Var;

				if (TryReflectTypeDesc(RefMember.type_description, Var))
				{
					Member = new PrimitiveBufferMember(RefMember.name, Var.Type, Var.ArraySize);
				}
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
	
	void ReflectShader(Ry::String ShaderLoc, Ry::ShaderStage Stage, ShaderReflection& OutReflectionData)
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

		// Reflect descriptor sets

		uint32 SetCount = 0;
		SpvReflectDescriptorSet** Sets = nullptr;		
		Result = spvReflectEnumerateDescriptorSets(&Module, &SetCount, nullptr);
		CORE_ASSERT(Result == SPV_REFLECT_RESULT_SUCCESS);

		if(SetCount > 0)
		{
			Sets = new SpvReflectDescriptorSet * [SetCount];
			Result = spvReflectEnumerateDescriptorSets(&Module, &SetCount, Sets);
			CORE_ASSERT(Result == SPV_REFLECT_RESULT_SUCCESS);

			//Ry::ArrayList<Ry::ResourceSetDescription*> ReflectedDescriptions;

			for (int32 SetIndex = 0; SetIndex < SetCount; SetIndex++)
			{
				SpvReflectDescriptorSet* Set = Sets[SetIndex];

				// Create new resource set description
				Ry::ResourceSetDescription* NewDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Stage }, Set->set);

				// Generate bindings for the set
				for (int32 BindingIndex = 0; BindingIndex < Set->binding_count; BindingIndex++)
				{
					SpvReflectDescriptorBinding& Binding = (*Set->bindings)[BindingIndex];

					int32 Index = Binding.binding;
					Ry::String Name = Binding.name;

					if (Binding.descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
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

				OutReflectionData.AddResourceDescription(NewDesc);
			}
		}
		

		// Reflect inputs

		uint32 InputVarCount = 0;
		SpvReflectInterfaceVariable** InputVars = nullptr;
		Result = spvReflectEnumerateInputVariables(&Module, &InputVarCount, nullptr);
		CORE_ASSERT(Result == SPV_REFLECT_RESULT_SUCCESS);

		if(InputVarCount > 0)
		{
			InputVars = new SpvReflectInterfaceVariable*[InputVarCount];
			Result = spvReflectEnumerateInputVariables(&Module, &InputVarCount, InputVars);
			CORE_ASSERT(Result == SPV_REFLECT_RESULT_SUCCESS);
		
			for(int32 InputVar = 0; InputVar < InputVarCount; InputVar++)
			{
				SpvReflectInterfaceVariable* Var = InputVars[InputVar];
				SpvReflectTypeDescription* Type = Var->type_description;

				if(Type->op != SpvOpTypeFloat && Type->op != SpvOpTypeVector)
				{
					Ry::Log->LogErrorf("Shader vertex input type not supported: %s", Type->type_name);
					continue;
				}

				ShaderVariable RefInputVar;
				if(TryReflectTypeDesc(Var->type_description, RefInputVar))
				{
					OutReflectionData.AddInputVariable(RefInputVar);
				}

			}
		}
		
		spvReflectDestroyShaderModule(&Module);

		delete[] OutSpirV;
		delete[] Sets;
		delete[] InputVars;

	}
	
}

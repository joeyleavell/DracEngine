#pragma once

#include "Core/Core.h"
#include "Interface/Shader.h"
#include "RenderingGen.h"

#define DeclPrimitive(DataType, Name) new Ry::PrimitiveBufferMember(Name, Ry::ShaderPrimitiveDataType::DataType, 1)
#define DeclStruct(Name, InitializerList) new Ry::StructBufferMember(Name, 1, InitializerList)
#define DeclPrimitiveArray(DataType, Name, ArraySize) new Ry::PrimitiveBufferMember(Name, ShaderPrimitiveDataType::DataType, ArraySize)
#define DeclStructArray(Name, ArraySize, InitializerList) new Ry::StructBufferMember(Name, ArraySize, InitializerList)

namespace Ry
{

	class SwapChain;
	class Texture;
	class FrameBuffer;
	struct ColorAttachment;

	class BufferMember
	{
	public:		
		Ry::String Id = "";
		int32 ArraySize = 1;

		BufferMember() = default;
		virtual ~BufferMember() = default;

	};

	class PrimitiveBufferMember : public BufferMember
	{
	public:
		ShaderPrimitiveDataType DataType;

		PrimitiveBufferMember()
		{
			
		}

		PrimitiveBufferMember(Ry::String Id, ShaderPrimitiveDataType DT, int32 ArraySize = 1)
		{
			this->Id = Id;
			this->DataType = DT;
			this->ArraySize = ArraySize;
		}

	};

	class StructBufferMember : public BufferMember
	{
	public:
		Ry::ArrayList<BufferMember*> StructMembers;

		StructBufferMember()
		{
			
		}

		StructBufferMember(Ry::String Id, int32 ArraySize, std::initializer_list<BufferMember*> Members)
		{
			this->Id = Id;
			this->ArraySize = ArraySize;
			
			for (auto Member : Members)
			{
				StructMembers.Add(Member);
			}
		}

		PrimitiveBufferMember& CreatePrimitiveMember(Ry::String Id, ShaderPrimitiveDataType Primitive, int32 ArraySize = 1)
		{
			PrimitiveBufferMember* Prim = new PrimitiveBufferMember;
			Prim->Id = Id;
			Prim->ArraySize = ArraySize;
			Prim->DataType = Primitive;

			return *Prim;
		}

		StructBufferMember& CreateStructMember(Ry::String Id, int32 ArraySize)
		{
			StructBufferMember* StructMember = new StructBufferMember;
			StructMembers.Add(StructMember);
			return *StructMember;
		}
		
	};

	class ConstantBuffer
	{
	public:
		int32 Index;
		Ry::String Name;
		Ry::ArrayList<BufferMember*> BufferMembers;

		ConstantBuffer(int32 Index, Ry::String Name)
		{
			this->Index = Index;
			this->Name = Name;
		}

		ConstantBuffer(int32 Index, Ry::String Name, std::initializer_list<BufferMember*> Members)
		{
			this->Index = Index;
			this->Name = Name;

			Create(Members);
		}

		void Create(std::initializer_list<BufferMember*> Members)
		{
			for(auto Member : Members)
			{
				BufferMembers.Add(Member);
			}
		}
		//
		// StructBufferMember& AddStructMember(Ry::String Id, int32 ArraySize)
		// {
		// 	StructBufferMember* BufferMember = new StructBufferMember;
		// 	BufferMembers.Add(BufferMember);
		// 	return *BufferMember;
		// }

	};

	class TextureBinding
	{
	public:
		int32 Index = 0;
		Ry::String Name;

		TextureBinding(int32 Index, Ry::String Name)
		{
			this->Index = Index;
			this->Name = Name;
		}
	};

	class RENDERING_MODULE ResourceLayout
	{
	public:

		int32 SetIndex;
		Ry::ArrayList<ShaderStage> Stages;
		Ry::ArrayList<ConstantBuffer*> ConstantBuffers;
		Ry::ArrayList<TextureBinding*> TextureBindings;

		ResourceLayout(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0)
		{
			this->SetIndex = SetIndex;
			this->Stages = Stages;
		}

		virtual ~ResourceLayout() {};

		void AddConstantBuffer(ConstantBuffer* Buffer)
		{
			ConstantBuffers.Add(Buffer);
		}
		
		void AddConstantBuffer(int32 Index, Ry::String Name, std::initializer_list<BufferMember*> Members)
		{
			ConstantBuffers.Add(new ConstantBuffer(Index, Name, Members));
		}

		void AddTextureBinding(TextureBinding* TexBinding)
		{
			TextureBindings.Add(TexBinding);
		}

		void AddTextureBinding(int32 Index, Ry::String Name)
		{
			TextureBindings.Add(new TextureBinding(Index, Name));
		}

		virtual void CreateDescription() = 0;
		virtual void DeleteDescription() = 0;

	};

	class RENDERING_MODULE ResourceSet
	{
	public:

		ResourceSet(const ResourceLayout* CreateInfo, SwapChain* SC) : Info(CreateInfo), Swap(SC) {};
		virtual ~ResourceSet() = default;

		virtual void CreateBuffer() = 0;
		virtual void DeleteBuffer() = 0;
		virtual bool Update() = 0;
		virtual void FlushBuffer(int32 Frame) = 0;

		virtual void BindFrameBufferAttachment(Ry::String TextureName, const Ry::ColorAttachment* ColorAttachment) = 0;
		virtual void BindTexture(Ry::String TextureName, const Ry::Texture* Resource) = 0;
		virtual void SetConstant(Ry::String BufferName, Ry::String Id, const void* Data) = 0;

		const ResourceLayout* GetInfo() const
		{
			return Info;
		}

		// Helper wrapper
		template <typename MatrixOrVectorType>
		void SetMatConstant(Ry::String Scene, Ry::String Id, MatrixOrVectorType MatrixOrVector)
		{
			SetConstant(Scene, Id, static_cast<const void*>(*MatrixOrVector));
		}

		void SetFloatConstant(Ry::String BufferName, Ry::String Id, float Value)
		{
			SetConstant(BufferName, Id, &Value);
		}
		
	protected:

		const ResourceLayout* Info;
		SwapChain* Swap;

	};
	
}
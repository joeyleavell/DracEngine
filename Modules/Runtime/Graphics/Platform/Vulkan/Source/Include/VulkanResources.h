#pragma once

#include "Core/Core.h"
#include "VulkanGen.h"
#include "Interface2/Shader2.h"
#include "Data/Map.h"
#include <cassert>
#include "vulkan/vulkan.h"
#include "Interface2/RenderingResource.h"

// Create offsets for Vulkan to be compatible with HLSL register locations
#define C_BUFFER_OFFSET 0
#define T_BUFFER_OFFSET 10
#define S_BUFFER_OFFSET 20
#define U_BUFFER_OFFSET 30

namespace Ry
{

	int32 CalcAlignmentRequirement(const BufferMember* Root);
	int32 CalcAlignmentRequirement(ShaderPrimitiveDataType Primitive);

	int32 CalcBufferSize(const ConstantBuffer* Buffer);
	void CalcMemberSize(const BufferMember* Layout, int32& Offset);

	int32 SizeOf(const ShaderPrimitiveDataType& PrimData);

	class UniformStorage
	{
		
	public:

		UniformStorage(bool bIsPrimitive, int32 Offset, uint8* Data, Ry::Map<Ry::String, UniformStorage*>& Children)
		{
			this->bIsPrimitive = bIsPrimitive;
			this->Offset = Offset;
			this->Data = Data;
			this->Children = Children;
		}

		UniformStorage(bool bIsPrimitive, int32 Offset, uint8* Data)
		{
			this->bIsPrimitive = bIsPrimitive;
			this->Offset = Offset;
			this->Data = Data;
		}

		template <typename MatrixOrVectorType>
		void Set(const MatrixOrVectorType& MatrixOrVector)
		{
			assert(bIsPrimitive);

			std::cout << "offset=" << Offset << std::endl;
			
			MemCpy(Data + Offset, sizeof(*MatrixOrVector), *MatrixOrVector, sizeof(*MatrixOrVector));
		}

		/**
		 * Sets a singular value.
		 *
		 * Note: must be a primitive value;
		 */
		template<Ry::ShaderPrimitiveDataType T>
		void Set(float* UniformData)
		{
			assert(bIsPrimitive);

			int32 DataTypeSize = SizeOf(T);

			MemCpy(Data + Offset, DataTypeSize, UniformData, DataTypeSize);
		}

		template <typename MatrixOrVectorType>
		UniformStorage& operator=(const MatrixOrVectorType& MatrixOrVector)
		{
			Set<MatrixOrVectorType>(MatrixOrVector);
			return *this;
		}

		UniformStorage& At(Ry::String Id, uint32 Index)
		{
			assert(Children.contains(Id));

			return *(*(Children.get(Id)));
		}

		UniformStorage& operator[](Ry::String Id)
		{
			assert(Children.contains(Id));

			return *(*(Children.get(Id)));
		}

	private:

		// Can only write to primitive data to prevent alignment 
		bool bIsPrimitive;
		
		uint32 Offset = 0;

		uint8* Data = nullptr;

		Ry::Map<Ry::String, UniformStorage*> Children;
	};


	class VulkanSwapChain;
	class VulkanBuffer;

	struct BufferRef
	{
		bool bIsPrimitive = false;
		int32 Offset = 0;
		int32 SlotSize = 0;
	};

	struct MappedConstantBuffer
	{
		int32 HostBufferSize;
		uint8* HostDataBuffer;
		Ry::Map<Ry::String, BufferRef> UniformRefs;

		/** The associated vulkan buffers for this constant buffer */
		Ry::ArrayList<VulkanBuffer*> VulkanBuffers;

		void Cleanup();
	};

	struct MappedTexture
	{
		VkImageView Image;
		VkSampler Sampler;
	};

	class VULKAN_MODULE VulkanResourceSetDescription : public Ry::ResourceSetDescription
	{
	public:

		VulkanResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0):
		ResourceSetDescription(Stages, SetIndex)
		{
			
		}

		void SetShaderFlags(VkDescriptorSetLayoutBinding& Binding);
		void CreateDescription() override;
		void DeleteDescription() override;

		const VkDescriptorSetLayout& GetVkLayout() const;

	private:
		VkDescriptorSetLayout SetLayout;

	};

	class VULKAN_MODULE VulkanResourceSet : public Ry::ResourceSet
	{
	public:

		std::vector<VkDescriptorSet> DescriptorSets;

		VulkanResourceSet(const ResourceSetDescription* CreateInfo, SwapChain* SC);

		void CreateBuffer() override;
		void DeleteBuffer() override;
		bool Update() override;
		void FlushBuffer(int32 Frame) override;

		void BindTexture(Ry::String TextureName, const Ry::Texture2* Resource) override;
		void SetConstant(Ry::String BufferName, Ry::String Id, const void* Data) override;

		void CreateUniformBuffers(VulkanSwapChain* SwapChain);
		void CreateDescriptorSets(VulkanSwapChain* SwapChain);
		void UpdateDescriptorSet(VulkanSwapChain* SwapChain, int32 SwapChainImageIndex);
		
		void PrintData();


	private:

		Ry::ArrayList<uint32> UniformDirtyFrames;

		bool bDirtyThisFrame = false;
		
		Ry::ArrayList<uint32> DirtyFrames;

		//std::vector<VulkanBuffer*> UniformBuffers;

		void MapData(MappedConstantBuffer* Dst, const ConstantBuffer* Src);
		void MapData_Helper(MappedConstantBuffer* Dst, const BufferMember* Member, Ry::String ParentId, int32& Marker); // Helper function

		void CreateBufferStorage();
		//UniformStorage* CreateBufferStorage(const BufferMember* DataLayout, uint8* Data, int32& Marker);

		void PrintTabs(int32 Tabs);
		void PrintData(const BufferMember* Member, int32 TabCount, int32& Offset);

		void SetShaderFlags(VkDescriptorSetLayoutBinding& Binding);

		Ry::Map<Ry::String, MappedConstantBuffer*> MappedConstantBuffers;
		Ry::Map<Ry::String, MappedTexture*> MappedTextures;

	};
	
}

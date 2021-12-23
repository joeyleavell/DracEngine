#pragma once

#include "Core/Core.h"
#include "VulkanGen.h"
#include "Shader.h"
#include "Data/Map.h"
#include "vulkan/vulkan.h"
#include "RenderingResource.h"

// Create offsets for Vulkan to be compatible with HLSL register locations
#define C_BUFFER_OFFSET 0
#define T_BUFFER_OFFSET 10
#define S_BUFFER_OFFSET 20
#define U_BUFFER_OFFSET 30

namespace Ry
{

	class VulkanSwapChain;
	class VulkanBuffer;

	struct MappedConstantBuffer
	{
		int32 HostBufferSize;
		uint8* HostDataBuffer;

		// Maps the uniform name to its offset in memory
		Ry::OAHashMap<Ry::String, BufferRef> UniformRefs;

		/** The associated vulkan buffers for this constant buffer */
		Ry::ArrayList<VulkanBuffer*> VulkanBuffers;

		void Cleanup();
	};

	struct MappedTexture
	{
		VkImageView Image;
		VkSampler Sampler;
	};

	class VULKAN_MODULE VulkanResourceSetDescription : public Ry::ResourceLayout
	{
	public:

		VulkanResourceSetDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0) : ResourceLayout(Stages, SetIndex){}

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

		Ry::ArrayList<VkDescriptorSet> DescriptorSets;

		VulkanResourceSet(const ResourceLayout* CreateInfo, SwapChain* SC);

		void CreateBuffer() override;
		void DeleteBuffer() override;
		bool Update() override;
		void FlushBuffer(int32 Frame) override;

		void BindFrameBufferAttachment(Ry::String TextureName, const Ry::ColorAttachment* ColorAttachment) override;
		void BindTexture(Ry::String TextureName, const Ry::Texture* Resource) override;
		void SetConstant(Ry::String BufferName, Ry::String Id, const void* Data) override;

		void BindTexture(const Ry::String TextureName, VkImageView Image, VkSampler Sampler);
		void CreateUniformBuffers(VulkanSwapChain* SwapChain);
		void CreateDescriptorSets(VulkanSwapChain* SwapChain);
		void UpdateDescriptorSet(VulkanSwapChain* SwapChain, int32 SwapChainImageIndex);
		
	private:

		bool bDirtyThisFrame = false;
		Ry::ArrayList<uint32> UniformDirtyFrames;
		Ry::ArrayList<uint32> DirtyFrames;
		Ry::OAHashMap<Ry::String, MappedConstantBuffer*> MappedConstantBuffers;
		Ry::OAHashMap<Ry::String, MappedTexture*> MappedTextures;

		void CreateBufferStorage();

	};
	
}

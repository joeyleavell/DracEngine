#pragma once

#include "Interface/FrameBuffer.h"
#include "vulkan/vulkan.h"
#include "VulkanGen.h"
#include "VulkanSwapChain.h"

namespace Ry
{

	class VULKAN_MODULE VulkanFrameBuffer : public Ry::FrameBuffer
	{
	public:

		VulkanFrameBuffer(uint32 Width, uint32 Height, const RenderPass* RenderPass, const FrameBufferDescription* Desc);
		virtual ~VulkanFrameBuffer() = default;

		void DeleteFramebuffer() override;
		void Recreate(uint32 Width, uint32 Height, const RenderPass* NewRenderPass) override;

		VkExtent2D GetFrameBufferExtent() const;
		VkFramebuffer GetFrameBufferForFrame(int32 FrameIndex);

	private:

		Ry::OAHashMap<uint32, VkImageView> CreatedColorImageViews;
		Ry::OAHashMap<uint32, VkDeviceMemory> CreatedColorDeviceMemory;
		Ry::OAHashMap<uint32, VkImage> CreatedColorImages;
		Ry::OAHashMap<uint32, VkSampler> CreatedColorSamplers;

		VkDeviceMemory CreatedDepthDeviceMemory;
		VkImageView CreatedDepthImageView;
		VkImage CreatedDepthImage;
		VkSampler CreatedDepthSampler;

		VulkanSwapChain* ReferencingSwapChain;

		void CreateFramebuffers(int32 Width, int32 Height, const VulkanRenderPass* VkRP);
		void DeleteFramebuffers();

		void OnSwapChainDirty();

		// This is an array because if we depend on a swap chain, we have to maintain one framebuffer per swap chain image/image view
		Ry::ArrayList<VkFramebuffer> FboResources;
		
	};
	
}

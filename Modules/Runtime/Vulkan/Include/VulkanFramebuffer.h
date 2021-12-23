#pragma once

#include "FrameBuffer.h"
#include "vulkan/vulkan.h"
#include "VulkanGen.h"
#include "VulkanSwapChain.h"

namespace Ry
{

	struct VulkanColorAttachment : public Ry::ColorAttachment
	{
		VkImageView ImageView;
		VkDeviceMemory DeviceMemory;
		VkImage Image;
		VkSampler Sampler;
	};

	class VULKAN_MODULE VulkanFrameBuffer : public Ry::FrameBuffer
	{
	public:

		VulkanFrameBuffer(uint32 Width, uint32 Height, const RenderPass* RenderPass, const FrameBufferDescription* Desc);
		virtual ~VulkanFrameBuffer() = default;

		void DeleteFramebuffer() override;
		void Recreate(uint32 Width, uint32 Height, const RenderPass* NewRenderPass) override;

		const ColorAttachment* GetColorAttachment(int32 AttachmentIndex) const override;

		VkExtent2D GetFrameBufferExtent() const;
		VkFramebuffer GetFrameBufferForFrame(int32 FrameIndex);

	private:

		Ry::OAHashMap<int32, VulkanColorAttachment*> CreatedColorAttachments;

		bool bCreatedDepthAttachment{};
		VkDeviceMemory CreatedDepthDeviceMemory;
		VkImageView CreatedDepthImageView;
		VkImage CreatedDepthImage;
		VkSampler CreatedDepthSampler;

		VulkanSwapChain* ReferencingSwapChain;

		void CreateFramebuffers(uint32 Width, uint32 Height, const VulkanRenderPass* VkRP);
		void DeleteFramebuffers();

		// This is an array because if we depend on a swap chain, we have to maintain one framebuffer per swap chain image/image view
		Ry::ArrayList<VkFramebuffer> FboResources;
		
	};
	
}

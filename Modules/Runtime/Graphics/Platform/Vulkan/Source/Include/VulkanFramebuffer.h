#pragma once

#include "Interface2/FrameBuffer2.h"
#include "vulkan/vulkan.h"
#include "VulkanGen.h"

namespace Ry
{

	class VULKAN_MODULE VulkanFrameBuffer : public Ry::FrameBuffer2
	{
	public:

		VulkanFrameBuffer(int32 Width, int32 Height);
		virtual ~VulkanFrameBuffer() = default;

		void DeleteFramebuffer() override;

		VkExtent2D GetFrameBufferExtent();

		void AddAttachment(VkImageView Attach);
		void CreateFrameBuffer(VkRenderPass RenderPass);
		VkFramebuffer GetResource();

	private:

		VkFramebuffer Resource;
		Ry::ArrayList<VkImageView> Attachments;
		
	};
	
}
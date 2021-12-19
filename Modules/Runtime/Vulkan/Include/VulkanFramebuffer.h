#pragma once

#include "Interface/FrameBuffer.h"
#include "vulkan/vulkan.h"
#include "VulkanGen.h"

namespace Ry
{

	class VULKAN_MODULE VulkanFrameBuffer : public Ry::FrameBuffer
	{
	public:

		VulkanFrameBuffer(int32 Width, int32 Height, const FrameBufferDescription* Desc);
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
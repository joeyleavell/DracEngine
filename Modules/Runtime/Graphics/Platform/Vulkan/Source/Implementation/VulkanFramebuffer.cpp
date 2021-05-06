#include "VulkanFramebuffer.h"
#include "VulkanContext.h"
#include "Core/Globals.h"

namespace Ry
{

	VulkanFrameBuffer::VulkanFrameBuffer(int32 Width, int32 Height) :
	FrameBuffer(Width, Height, 1)
	{
	}

	void VulkanFrameBuffer::DeleteFramebuffer()
	{
		vkDestroyFramebuffer(GVulkanContext->GetLogicalDevice(), Resource, nullptr);
	}

	VkExtent2D VulkanFrameBuffer::GetFrameBufferExtent()
	{
		return VkExtent2D{GetIntendedWidth(), GetIntendedHeight()};
	}

	void VulkanFrameBuffer::AddAttachment(VkImageView Attach)
	{
		Attachments.Add(Attach);
	}

	void VulkanFrameBuffer::CreateFrameBuffer(VkRenderPass RenderPass)
	{
		VkFramebufferCreateInfo FramebufferInfo{};
		FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferInfo.renderPass = RenderPass;
		FramebufferInfo.attachmentCount = Attachments.GetSize();
		FramebufferInfo.pAttachments = Attachments.GetData();
		FramebufferInfo.width = GetIntendedWidth();
		FramebufferInfo.height = GetIntendedHeight();
		FramebufferInfo.layers = 1;

		if (vkCreateFramebuffer(GVulkanContext->GetLogicalDevice(), &FramebufferInfo, nullptr, &Resource) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create Vulkan framebuffer");
		}		
	}

	VkFramebuffer VulkanFrameBuffer::GetResource()
	{
		return Resource;
	}
	
}

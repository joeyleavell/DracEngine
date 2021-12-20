#include "VulkanFramebuffer.h"
#include "VulkanContext.h"
#include "Core/Globals.h"

namespace Ry
{

	VulkanFrameBuffer::VulkanFrameBuffer(uint32 Width, uint32 Height, const RenderPass* RenderPass, const FrameBufferDescription* Desc) :
	FrameBuffer(Width, Height, RenderPass, Desc)
	{
		ReferencingSwapChain = nullptr;

		if(Desc)
		{
			for (const AttachmentDescription& AttachDesc : Desc->Attachments)
			{
				if (AttachDesc.ReferencingSwapChain)
				{
					if(ReferencingSwapChain && ReferencingSwapChain != AttachDesc.ReferencingSwapChain)
					{
						Ry::Log->LogError("VulkanFrameBuffer::VulkanFrameBuffer: Vulkan framebuffer references multiple swap chains, this is not supported");
					}

					if(VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(AttachDesc.ReferencingSwapChain))
					{
						ReferencingSwapChain = VkSC;
					}
					else
					{
						Ry::Log->LogError("VulkanFrameBuffer::VulkanFrameBuffer: Passed in non vulkan swap chain");
					}
				}
			}
		}

		// Initialize framebuffers
		if(const VulkanRenderPass* VkRP = dynamic_cast<const VulkanRenderPass*>(RenderPass))
		{
			CreateFramebuffers(Width, Height, VkRP);
		}
		else
		{
			Ry::Log->LogError("VulkanFrameBuffer::VulkanFrameBuffer: Passed in non vulkan render pass");
		}
	}

	void VulkanFrameBuffer::DeleteFramebuffer()
	{
		// Destroy framebuffers and clear array
		for (int32 FbIndex = 0; FbIndex < FboResources.GetSize(); FbIndex++)
		{
			vkDestroyFramebuffer(GVulkanContext->GetLogicalDevice(), FboResources[FbIndex], nullptr);
		}

		FboResources.Clear();
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

	VkFramebuffer VulkanFrameBuffer::GetFrameBufferForFrame(int32 FrameIndex)
	{
		if(ReferencingSwapChain)
		{
			return FboResources[FrameIndex];
		}
		else
		{
			// There is only one resource if we don't depend on the swap chain
			return FboResources[0];
		}
	}

	void VulkanFrameBuffer::Recreate(uint32 Width, uint32 Height, const RenderPass* NewRenderPass)
	{
		if (const VulkanRenderPass* VkRP = dynamic_cast<const VulkanRenderPass*>(NewRenderPass))
		{
			// Re-create the framebuffers
			DeleteFramebuffers();
			CreateFramebuffers(Width, Height, VkRP);
		}
		else
		{
			Ry::Log->LogError("VulkanFrameBuffer::VulkanFrameBuffer: Passed in non vulkan render pass");
		}

	}

	VkExtent2D VulkanFrameBuffer::GetFrameBufferExtent() const
	{
		return VkExtent2D{ GetIntendedWidth(), GetIntendedHeight() };
	}

	void VulkanFrameBuffer::CreateFramebuffers(int32 Width, int32 Height, const VulkanRenderPass* VkRP)
	{
		this->IntendedWidth = Width;
		this->IntendedHeight = Height;

		if(ReferencingSwapChain)
		{
			// Resize frame-buffers to correct amount
			FboResources.SetSize(ReferencingSwapChain->SwapChainImageViews.GetSize());

			for (uint32 ImageIndex = 0; ImageIndex < ReferencingSwapChain->SwapChainImageViews.GetSize(); ImageIndex++)
			{
				Ry::ArrayList<VkImageView> Attachments;

				bool bUseDepthStencil = false;
				bool bUseSwapDeptchStencil = false;

				for (const AttachmentDescription& Desc : Description.Attachments)
				{
					if(Desc.Format == AttachmentFormat::Color)
					{
						if(Desc.ReferencingSwapChain)
						{
							// Simply use the image view from the swap chain
							VkImageView& SwapChainImageView = ReferencingSwapChain->SwapChainImageViews[ImageIndex];
							Attachments.Add(SwapChainImageView);
						}
						else
						{
							// Create new device memory, image, and image view for this attachment
							Ry::Log->LogError("Needs implementing");
						}
					}
					else if(Desc.Format == AttachmentFormat::Depth || Desc.Format == AttachmentFormat::Stencil)
					{
						// The creation of these are deferred
						bUseDepthStencil = true;
						if(Desc.ReferencingSwapChain)
						{
							bUseSwapDeptchStencil = true;
						}
					}
				}

				if(bUseDepthStencil)
				{
					if(bUseSwapDeptchStencil)
					{
						VkImageView& SwapChainImageView = ReferencingSwapChain->DepthImageView;
						Attachments.Add(SwapChainImageView);
					}
					else
					{
						// Create new device memory, image, and image view for this attachment
						Ry::Log->LogError("Needs implementing");
					}
				}

				VkFramebufferCreateInfo FramebufferInfo{};
				FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				FramebufferInfo.renderPass = VkRP->GetRenderPass();
				FramebufferInfo.attachmentCount = Attachments.GetSize();
				FramebufferInfo.pAttachments = Attachments.GetData();
				FramebufferInfo.width = IntendedWidth;
				FramebufferInfo.height = IntendedHeight;
				FramebufferInfo.layers = 1;

				if (vkCreateFramebuffer(GVulkanContext->GetLogicalDevice(), &FramebufferInfo, nullptr, &FboResources[ImageIndex]) != VK_SUCCESS)
				{
					Ry::Log->LogError("Failed to create Vulkan framebuffer");
				}

			}
		}

	}

	void VulkanFrameBuffer::DeleteFramebuffers()
	{
	}

	void VulkanFrameBuffer::OnSwapChainDirty()
	{

		// Whenever the swapchain goes dirty, we have to re-create the internal framebuffer objects
	}

}

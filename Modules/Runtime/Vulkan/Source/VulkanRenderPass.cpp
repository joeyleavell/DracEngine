#include "VulkanRenderPass.h"
#include "VulkanContext.h"
#include "Core/Globals.h"

namespace Ry
{

	void VulkanRenderPass::DeleteRenderPass()
	{
		vkDestroyRenderPass(GVulkanContext->GetLogicalDevice(), RenderPass, nullptr);
	}
	
	bool VulkanRenderPass::CreateRenderPass()
	{
		if(SubPasses.GetSize() != 1)
		{
			Ry::Log->LogError("Currently only support a single Vulkan subpass");
			return false;
		}
		
		Ry::ArrayList<VkAttachmentDescription> VkAttachmentDescriptions;
		Ry::ArrayList<VkSubpassDescription> VkSubpasses;
		Ry::ArrayList<VkAttachmentReference*> MemoryToFree;

		for (AttachmentDescription& Desc : Description.Attachments)
		{
			VulkanSwapChain* VkSwapChain = dynamic_cast<VulkanSwapChain*>(Desc.ReferencingSwapChain);

			if (Desc.ReferencingSwapChain && !VkSwapChain)
			{
				Ry::Log->LogError("Passed in a non vulkan swap chain to a vulkan render pass");
				return false;
			}

			VkAttachmentDescription NewAttachment{};
			NewAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			NewAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			NewAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			NewAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			if (Desc.Format == AttachmentFormat::Color)
			{
				NewAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				NewAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

				if (Desc.ReferencingSwapChain)
				{
					NewAttachment.format = VkSwapChain->SwapChainImageFormat;
					NewAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				}
				else
				{
					NewAttachment.format = VK_FORMAT_R8G8B8A8_SRGB; // Standard format of custom color attachments
					NewAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				}
			}
			else if (Desc.Format == AttachmentFormat::Depth)
			{
				if(Desc.ReferencingSwapChain)
				{
					// Find the best format for a depth attachment
					FindDepthFormat(NewAttachment.format);
				}
				else
				{
					NewAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT; // Standard format of custom depth attachments
				}

				NewAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				NewAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				NewAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}

			VkAttachmentDescriptions.Add(NewAttachment);
		}

		for(Subpass* SubPass : SubPasses)
		{
			Ry::ArrayList<VkAttachmentReference> ColorAttachmentRefs;
			Ry::ArrayList<VkAttachmentReference> DepthStencilAttachmentRefs;

			for(int32 RefIndex = 0; RefIndex < SubPass->UsedAttachments.GetSize(); RefIndex++)
			{
				AttachmentDescription AttachDesc = Description.Attachments[SubPass->UsedAttachments[RefIndex]];

				VkAttachmentReference AttachmentRef{};
				AttachmentRef.attachment = RefIndex; // Set the attachment index to the reference index

				// Set the optimal layout for this subpass
				if(AttachDesc.Format == AttachmentFormat::Color)
				{
					AttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					ColorAttachmentRefs.Add(AttachmentRef);
				}
				if (AttachDesc.Format == AttachmentFormat::Depth)
				{
					AttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					DepthStencilAttachmentRefs.Add(AttachmentRef);

					if(DepthStencilAttachmentRefs.GetSize() > 1)
					{
						Ry::Log->LogError("More than one depth pass specified, only 1 depth pass supported");
						return false;
					}
				}

			}

			VkAttachmentReference* ColorAttachmentsPtr = ColorAttachmentRefs.CopyData();

			// Create the new subpass (always bind to graphics for now)
			VkSubpassDescription Subpass{};
			Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			Subpass.colorAttachmentCount = ColorAttachmentRefs.GetSize();
			Subpass.pColorAttachments = ColorAttachmentsPtr;
			MemoryToFree.Add(ColorAttachmentsPtr);

			if(DepthStencilAttachmentRefs.GetSize() > 0)
			{
				VkAttachmentReference* DepthAttachmentPtr = DepthStencilAttachmentRefs.CopyData();
				Subpass.pDepthStencilAttachment = DepthAttachmentPtr;
				MemoryToFree.Add(DepthAttachmentPtr);
			}

			VkSubpasses.Add(Subpass);
		}

		// Create single dependency for now
		VkSubpassDependency Dependency{};
		Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass = 0;
		Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		Dependency.srcAccessMask = 0;
		Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = VkAttachmentDescriptions.GetSize();
		RenderPassInfo.pAttachments = VkAttachmentDescriptions.GetData();
		RenderPassInfo.subpassCount = VkSubpasses.GetSize();
		RenderPassInfo.pSubpasses = VkSubpasses.GetData();
		RenderPassInfo.dependencyCount = 1;
		RenderPassInfo.pDependencies = &Dependency;
		RenderPassInfo.flags = 0;

		bool bSuccess = true;

		if (vkCreateRenderPass(GVulkanContext->GetLogicalDevice(), &RenderPassInfo, nullptr, &RenderPass) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create Vulkan render pass");
			bSuccess = false;
		}

		// Delete dynamically allocated memory
		for (VkAttachmentReference* Mem : MemoryToFree)
		{
			delete[] Mem;
		}

		return bSuccess;
	}

	VkRenderPass VulkanRenderPass::GetRenderPass() const
	{
		return RenderPass;
	}
}

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

	const ColorAttachment* VulkanFrameBuffer::GetColorAttachment(int32 AttachmentIndex) const
	{
		if(CreatedColorAttachments.Contains(AttachmentIndex))
		{
			return CreatedColorAttachments.Get(AttachmentIndex);
		}

		return nullptr;
	}

	VkExtent2D VulkanFrameBuffer::GetFrameBufferExtent() const
	{
		return VkExtent2D{ GetIntendedWidth(), GetIntendedHeight() };
	}

	void VulkanFrameBuffer::CreateFramebuffers(int32 Width, int32 Height, const VulkanRenderPass* VkRP)
	{
		this->IntendedWidth = Width;
		this->IntendedHeight = Height;

		// Create non swap based attachments up here
		for (int32 AttachDescIndex = 0; AttachDescIndex < Description.Attachments.GetSize(); AttachDescIndex++)
		{
			AttachmentDescription& Desc = Description.Attachments[AttachDescIndex];

			// Only create attachments if there is no referencing swap chain or existing attachment
			if (Desc.ReferencingSwapChain || Desc.ExistingAttachment)
				continue;

			VulkanColorAttachment* NewAttachment = new VulkanColorAttachment;
			// VkImage ResultImage;
			// VkImageView ResultImageView;
			// VkDeviceMemory ResultMemory;
			// VkSampler ResultSampler;

			VkFormat ImageFormat;
			VkImageUsageFlags UsageFlags;
			VkImageAspectFlags AspectFlags;

			if(Desc.Format == AttachmentFormat::Color)
			{
				ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;

				// Color attachment, and sampling in case attachment is used as input
				UsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

				AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
			}
			else
			{
				// Format for both depth and stencil buffer
				ImageFormat = VK_FORMAT_D24_UNORM_S8_UINT;

				// Color attachment, and sampling in case attachment is used as input
				UsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

				// This is used as both a depth and stencil buffer
				AspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			if (!CreateImage(
				GetIntendedWidth(),
				GetIntendedHeight(),
				ImageFormat,
				VK_IMAGE_TILING_OPTIMAL,
				UsageFlags, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				NewAttachment->Image,
				NewAttachment->DeviceMemory
			))
			{
				Ry::Log->LogError("Failed to create a color image");
			}

			if (!CreateImageView(
				NewAttachment->ImageView,
				NewAttachment->Image,
				ImageFormat,
				AspectFlags
			))
			{
				Ry::Log->LogError("Failed to create an image view for the depth image");
			}

			// Create sampler to use when reading this attachment
			VkSamplerCreateInfo SamplerInfo{};
			SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			SamplerInfo.magFilter = VK_FILTER_NEAREST; // Always use nearest filtering for attachments
			SamplerInfo.minFilter = VK_FILTER_NEAREST; // Always use nearest filtering for attachments
			SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			SamplerInfo.anisotropyEnable = VK_FALSE;
			SamplerInfo.maxAnisotropy = 0.0f;
			SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			SamplerInfo.unnormalizedCoordinates = VK_FALSE;
			SamplerInfo.compareEnable = VK_FALSE;
			SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			SamplerInfo.mipLodBias = 0.0f;
			SamplerInfo.minLod = 0.0f;
			SamplerInfo.maxLod = 0.0f;

			if (vkCreateSampler(GVulkanContext->GetLogicalDevice(), &SamplerInfo, nullptr, &NewAttachment->Sampler) != VK_SUCCESS)
			{
				Ry::Log->LogError("Failed to create texture sampler");
			}

			if (Desc.Format == AttachmentFormat::Color)
			{
				// Only insert the color attachment for created attachments
				CreatedColorAttachments.Insert(AttachDescIndex, NewAttachment);
			}
			else
			{
				// There can only be a single depth/stencil attachment
				// For now, we're just copying over from the "color" attachment
				CreatedDepthDeviceMemory = NewAttachment->DeviceMemory;
				CreatedDepthImageView = NewAttachment->ImageView;
				CreatedDepthImage = NewAttachment->Image;
				CreatedDepthSampler = NewAttachment->Sampler;
			}

		}

		// Resize frame-buffers to correct amount
		int32 ResourceCount = ReferencingSwapChain ? ReferencingSwapChain->SwapChainImageViews.GetSize() : 1;

		for (uint32 ImageIndex = 0; ImageIndex < ResourceCount; ImageIndex++)
		{
			Ry::ArrayList<VkImageView> Attachments;

			for (int32 AttachDescIndex = 0; AttachDescIndex < Description.Attachments.GetSize(); AttachDescIndex++)
			{
				const AttachmentDescription& Desc = Description.Attachments[AttachDescIndex];

				if(Desc.Format == AttachmentFormat::Color)
				{
					if(Desc.ReferencingSwapChain)
					{
						// Simply use the image view from the swap chain
						VkImageView& SwapChainImageView = ReferencingSwapChain->SwapChainImageViews[ImageIndex];
						Attachments.Add(SwapChainImageView);
					}
					else if(Desc.ExistingAttachment)
					{
						if(const VulkanColorAttachment* VkCA = dynamic_cast<const VulkanColorAttachment*>(Desc.ExistingAttachment))
						{
							const VkImageView& ExistingImageView = VkCA->ImageView;
							Attachments.Add(ExistingImageView);
						}
						else
						{
							Ry::Log->LogError("VulkanFrameBuffer::CreateFramebuffers: Invalid color attachment");
						}
					}
					else if(CreatedColorAttachments.Contains(AttachDescIndex))
					{
						VkImageView& CreatedImageView = CreatedColorAttachments.Get(AttachDescIndex)->ImageView;
						Attachments.Add(CreatedImageView);
					}
				}
				else if(Desc.Format == AttachmentFormat::Depth || Desc.Format == AttachmentFormat::Stencil)
				{
					// The creation of these are deferred
					if(Desc.ReferencingSwapChain)
					{
						VkImageView& SwapChainImageView = ReferencingSwapChain->DepthImageView;
						Attachments.Add(SwapChainImageView);
					}
					else
					{
						Attachments.Add(CreatedDepthImageView);
					}
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

	void VulkanFrameBuffer::DeleteFramebuffers()
	{
		// todo: destroy all resources
	}

	void VulkanFrameBuffer::OnSwapChainDirty()
	{

		// Whenever the swapchain goes dirty, we have to re-create the internal framebuffer objects
	}

}

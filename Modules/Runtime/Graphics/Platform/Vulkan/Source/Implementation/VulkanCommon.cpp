#include "VulkanCommon.h"
#include "Core/Globals.h"
#include "VulkanContext.h"
#include <set>

namespace Ry
{

	void FindQueueFamiliesForDevice(QueueFamilies& OutQueues, VkPhysicalDevice Device, VkSurfaceKHR Surface)
	{
		uint32_t QueueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);

		VkQueueFamilyProperties* FamilyProperies = new VkQueueFamilyProperties[QueueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, FamilyProperies);

		for (uint32 QueueFamIndex = 0; QueueFamIndex < QueueFamilyCount; QueueFamIndex++)
		{
			VkQueueFamilyProperties FamProps = FamilyProperies[QueueFamIndex];

			// See if this queue family can present
			VkBool32 PresentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(Device, QueueFamIndex, Surface, &PresentSupport);

			if (PresentSupport)
			{
				OutQueues.PresentFamily = QueueFamIndex;
			}

			// See if this queue family has graphics support
			if (FamProps.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				OutQueues.GraphicsFamily = QueueFamIndex;
			}
		}

		delete[] FamilyProperies;

	}

	uint32 FindMemoryType(uint32 TypeFilter, VkMemoryPropertyFlags Props)
	{
		VkPhysicalDeviceMemoryProperties MemProperties;
		vkGetPhysicalDeviceMemoryProperties(GVulkanContext->GetPhysicalDevice(), &MemProperties);

		for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
		{
			if (TypeFilter & (1 << i) && (MemProperties.memoryTypes[i].propertyFlags & Props) == Props)
			{
				return i;
			}
		}

		Ry::Log->LogError("Could not find proper memory type");
		
		return -1;
	}

	void CopyBufferToImage(uint32 Width, uint32 Height, VkBuffer& SrcBuffer, VkImage& DstImage)
	{
		VulkanCommandBuffer CmdBuffer(nullptr);
		CmdBuffer.CreateBuffer(GVulkanContext->GetCommandPool());

		CmdBuffer.BeginRecording(true);
		{
			VkBufferImageCopy Region;
			Region.bufferOffset = 0;
			Region.bufferRowLength = 0;
			Region.bufferImageHeight = 0;

			Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			Region.imageSubresource.mipLevel = 0;
			Region.imageSubresource.baseArrayLayer = 0;
			Region.imageSubresource.layerCount = 1;

			Region.imageOffset = { 0, 0, 0 };
			Region.imageExtent = {
				Width,
				Height,
				1
			};

			vkCmdCopyBufferToImage(
				*CmdBuffer.GetBuffer(),
				SrcBuffer,
				DstImage,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&Region
			);

		}
		CmdBuffer.EndRecording();
	}

	void TransitionImageLayout(VkImage& Image, VkImageLayout OldLayout, VkImageLayout NewLayout)
	{
		VulkanCommandBuffer CmdBuffer(nullptr);
		CmdBuffer.CreateBuffer(GVulkanContext->GetCommandPool());

		CmdBuffer.BeginRecording(true);
		{
			VkImageMemoryBarrier Barrier{};
			Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			Barrier.oldLayout = OldLayout;
			Barrier.newLayout = NewLayout;
			Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			Barrier.image = Image;
			Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			Barrier.subresourceRange.baseMipLevel = 0;
			Barrier.subresourceRange.levelCount = 1;
			Barrier.subresourceRange.baseArrayLayer = 0;
			Barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags SourceStage;
			VkPipelineStageFlags DestinationStage;

			if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				Barrier.srcAccessMask = 0;
				Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else
			{
				Ry::Log->LogError("Unsupported vulkan layout transition"); // todo: support all layout transitions we care about
				return;
			}

			vkCmdPipelineBarrier(
				*CmdBuffer.GetBuffer(),
				SourceStage, DestinationStage,
				0,
				0, nullptr,
				0, nullptr,
				1, &Barrier
			);
		}
		CmdBuffer.EndRecording();
	}

	bool CreateImage(uint32 Width, uint32 Height, VkFormat ImageFormat, VkImageTiling Tiling, VkImageUsageFlags Usage, VkMemoryPropertyFlags Props, VkImage& OutImage, VkDeviceMemory& OutMemory)
	{
		VkImageCreateInfo ImageInfo{};
		ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ImageInfo.imageType = VK_IMAGE_TYPE_2D;
		ImageInfo.extent.width = Width;
		ImageInfo.extent.height = Height;
		ImageInfo.extent.depth = 1;
		ImageInfo.mipLevels = 1;
		ImageInfo.arrayLayers = 1;
		ImageInfo.format = ImageFormat;
		ImageInfo.tiling = Tiling;
		ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ImageInfo.usage = Usage;
		ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		ImageInfo.flags = 0; // Optional

		if (vkCreateImage(GVulkanContext->GetLogicalDevice(), &ImageInfo, nullptr, &OutImage) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create Vulkan texture");
			return false;
		}

		VkMemoryRequirements MemRequirements;
		vkGetImageMemoryRequirements(GVulkanContext->GetLogicalDevice(), OutImage, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocInfo.allocationSize = MemRequirements.size;
		AllocInfo.memoryTypeIndex = Ry::FindMemoryType(MemRequirements.memoryTypeBits, Props);

		if (vkAllocateMemory(GVulkanContext->GetLogicalDevice(), &AllocInfo, nullptr, &OutMemory) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to allocate Vulkan texture memory");
			return false;
		}

		vkBindImageMemory(GVulkanContext->GetLogicalDevice(), OutImage, OutMemory, 0);

		return true;
	}

	bool CreateImageView(VkImageView& OutView, const VkImage& ForImage, VkFormat Format, VkImageAspectFlags AspectFlags)
	{
		VkImageViewCreateInfo ViewInfo{};
		ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ViewInfo.image = ForImage;
		ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ViewInfo.format = Format;
		ViewInfo.subresourceRange.aspectMask = AspectFlags;
		ViewInfo.subresourceRange.baseMipLevel = 0;
		ViewInfo.subresourceRange.levelCount = 1;
		ViewInfo.subresourceRange.baseArrayLayer = 0;
		ViewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(GVulkanContext->GetLogicalDevice(), &ViewInfo, nullptr, &OutView) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create vulkan texture");
			return false;
		}

		return true;		
	}

	bool FindOptimalDepthFormat(VkFormat& OutFormat, const std::vector<VkFormat>& Candidates, VkImageTiling Tiling, VkFormatFeatureFlags Features)
	{
		for (VkFormat Format : Candidates)
		{
			VkFormatProperties Props;
			vkGetPhysicalDeviceFormatProperties(GVulkanContext->GetPhysicalDevice(), Format, &Props);

			if (Tiling == VK_IMAGE_TILING_LINEAR && (Props.linearTilingFeatures & Features) == Features)
			{
				OutFormat = Format;
				return true;
			}
			else if (Tiling == VK_IMAGE_TILING_OPTIMAL && (Props.optimalTilingFeatures & Features) == Features)
			{
				OutFormat = Format;
				return true;
			}
		}

		return false;
	}

	bool FindDepthFormat(VkFormat& OutFormat)
	{
		return FindOptimalDepthFormat(OutFormat,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	bool HasStencilComponent(VkFormat Format)
	{
		return Format == VK_FORMAT_D32_SFLOAT_S8_UINT || Format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	bool CheckDeviceExtensionSupport(VkPhysicalDevice PhysicalDev, const Ry::ArrayList<Ry::String>& Extensions)
	{
		uint32 ExtensionCount;
		vkEnumerateDeviceExtensionProperties(PhysicalDev, nullptr, &ExtensionCount, nullptr);

		// Ext count is a pointer in case we didn't already know the number of extensions
		VkExtensionProperties* SupportedExtensions = new VkExtensionProperties[ExtensionCount];
		vkEnumerateDeviceExtensionProperties(PhysicalDev, nullptr, &ExtensionCount, SupportedExtensions);

		Ry::ArrayList<Ry::String> SupportedExtensionsSet;
		for (uint32 ExtPropIndex = 0; ExtPropIndex < ExtensionCount; ExtPropIndex++)
		{
			char* ExtName = SupportedExtensions[ExtPropIndex].extensionName;
			SupportedExtensionsSet.Add(ExtName);
		}

		// Remove all required extensions that are available.
		for (const Ry::String& RequiredExt : Extensions)
		{
			if (!SupportedExtensionsSet.Contains(RequiredExt))
			{
				return false;
			}
		}

		return true;
	}

	bool CheckValidationLayerSupport(const Ry::ArrayList<Ry::String>& Layers)
	{
		uint32_t LayerCount;
		vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);

		std::vector<VkLayerProperties> AvailableLayers(LayerCount);
		vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

		// Check that all of the validation layers are available
		for (const Ry::String& ValidationLayer : Layers)
		{
			bool LayerFound = false;

			for (const auto& LayerProps : AvailableLayers)
			{
				if (ValidationLayer == LayerProps.layerName)
				{
					LayerFound = true;
					break;
				}
			}

			if (!LayerFound)
			{
				return false;
			}
		}

		return true;
	}

	
}

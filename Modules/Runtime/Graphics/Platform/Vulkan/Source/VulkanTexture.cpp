#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "Bitmap.h"
#include "Core/Globals.h"
#include "VulkanContext.h"
#include "VulkanCommandBuffer.h"

namespace Ry
{
	
	VulkanTexture::VulkanTexture(TextureUsage InUsage, TextureFiltering Filter):
	Texture(InUsage, Filter)
	{
		this->StagingMemory = nullptr;
	}

	VulkanTexture::~VulkanTexture()
	{
		
	}

	void VulkanTexture::DeleteTexture()
	{
		vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());
		
		vkDestroySampler(GVulkanContext->GetLogicalDevice(), TextureSampler, nullptr);
		
		vkDestroyImageView(GVulkanContext->GetLogicalDevice(), TextureImageView, nullptr);
		
		if(StagingMemory)
		{
			delete StagingMemory;
		}
		
		vkDestroyImage(GVulkanContext->GetLogicalDevice(), TextureResource, nullptr);
		vkFreeMemory(GVulkanContext->GetLogicalDevice(), TextureMemory, nullptr);
	}

	void VulkanTexture::Data(const Bitmap* Bitmap)
	{
		Texture::Data(Bitmap);

		/*if(Bitmap->GetPixelBuffer()->GetPixelStorage() != PixelStorage::THREE_BYTE_RGB)
		{
			Ry::Log->LogError("Vulkan textures only support three byte RGB bitmaps");
			return;
		}*/

		// Todo: support more vulkan formats

		VkDeviceSize ImageSizeBytes = Bitmap->GetWidth() * Bitmap->GetHeight() * 4;

		// todo: utilize texture usage
		StagingMemory = new VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ImageSizeBytes);

		uint32* RGBAData = Bitmap->GetPixelBuffer()->GetAsRGBA();
		StagingMemory->UploadData(RGBAData, Bitmap->GetWidth() * Bitmap->GetHeight());
		delete RGBAData; // Don't need data anymore

		VkImageCreateInfo ImageInfo{};
		ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		ImageInfo.imageType = VK_IMAGE_TYPE_2D;
		ImageInfo.extent.width = static_cast<uint32_t>(Bitmap->GetWidth());
		ImageInfo.extent.height = static_cast<uint32_t>(Bitmap->GetHeight());
		ImageInfo.extent.depth = 1;
		ImageInfo.mipLevels = 1;
		ImageInfo.arrayLayers = 1;
		ImageInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // Always upload the image in R8G8B8A8 SRGB format
		ImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		ImageInfo.flags = 0; // Optional

		if (vkCreateImage(GVulkanContext->GetLogicalDevice(), &ImageInfo, nullptr, &TextureResource) != VK_SUCCESS) 
		{
			Ry::Log->LogError("Failed to create Vulkan texture");
			return;
		}

		VkMemoryRequirements MemRequirements;
		vkGetImageMemoryRequirements(GVulkanContext->GetLogicalDevice(), TextureResource, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocInfo.allocationSize = MemRequirements.size;
		AllocInfo.memoryTypeIndex = Ry::FindMemoryType(MemRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(GVulkanContext->GetLogicalDevice(), &AllocInfo, nullptr, &TextureMemory) != VK_SUCCESS) 
		{
			Ry::Log->LogError("Failed to allocate Vulkan texture memory");
			return;
		}

		vkBindImageMemory(GVulkanContext->GetLogicalDevice(), TextureResource, TextureMemory, 0);

		TransitionImageLayout(TextureResource, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL); 	// Transition to optimal layout for transferring image
		CopyBufferToImage(Bitmap->GetWidth(), Bitmap->GetHeight(), StagingMemory->GetBufferObject(), TextureResource);
		TransitionImageLayout(TextureResource, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); // Transition to being shader read only

		// Now create an image view
		VkImageViewCreateInfo ViewInfo{};
		ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ViewInfo.image = TextureResource;
		ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ViewInfo.format = ImageInfo.format; // Use same format as texture image
		ViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ViewInfo.subresourceRange.baseMipLevel = 0;
		ViewInfo.subresourceRange.levelCount = 1;
		ViewInfo.subresourceRange.baseArrayLayer = 0;
		ViewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(GVulkanContext->GetLogicalDevice(), &ViewInfo, nullptr, &TextureImageView) != VK_SUCCESS) 
		{
			Ry::Log->LogError("Failed to create vulkan texture");
			return;
		}

		VkFilter VulkanFilter = VK_FILTER_LINEAR;
		if (GetFilter() == TextureFiltering::Nearest)
			VulkanFilter = VK_FILTER_NEAREST;

		// Now create a sampler to use with this texture
		VkSamplerCreateInfo SamplerInfo{};
		SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		SamplerInfo.magFilter = VulkanFilter;
		SamplerInfo.minFilter = VulkanFilter;
		SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		SamplerInfo.anisotropyEnable = VK_FALSE;

		// Todo: move physical device query elsewhere
		//VkPhysicalDeviceProperties Properties{};
		//vkGetPhysicalDeviceProperties(GVulkanContext->GetPhysicalDevice(), &Properties);
		SamplerInfo.maxAnisotropy = 0.0f;//Properties.limits.maxSamplerAnisotropy;
		
		SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		SamplerInfo.unnormalizedCoordinates = VK_FALSE;
		SamplerInfo.compareEnable = VK_FALSE;
		SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		SamplerInfo.mipLodBias = 0.0f;
		SamplerInfo.minLod = 0.0f;
		SamplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(GVulkanContext->GetLogicalDevice(), &SamplerInfo, nullptr, &TextureSampler) != VK_SUCCESS) 
		{
			Ry::Log->LogError("Failed to create texture sampler");
		}
		
	}

}

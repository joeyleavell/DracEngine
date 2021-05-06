#pragma once

#include "VulkanGen.h"
#include "Interface/Texture.h"
#include "vulkan/vulkan.h"

namespace Ry
{

	class VulkanBuffer;

	class VULKAN_MODULE VulkanTexture : public Ry::Texture
	{

	public:

		VulkanTexture(TextureUsage InUsage);
		virtual ~VulkanTexture();

		virtual void DeleteTexture();

		void Data(const Bitmap* Bitmap);
		virtual void Bind(uint32 Unit) {};
		virtual int32 GetSamples() const { return 0; };

		/**
		 * Sets this texture up as a blank buffer with the specified width and height.
		 * @param width The width of the texture.
		 * @param height The height of the texture.
		 */
		void AllocateColor(uint32 width, uint32 height){};
		void AllocateDepth(uint32 width, uint32 height){};

		void Bind(){};

		VkImage GetResource() const
		{
			return TextureResource;
		}

		VkImageView GetImageView() const
		{
			return TextureImageView;
		}

		VkSampler GetSampler() const
		{
			return TextureSampler;
		}

	private:

		VulkanBuffer* StagingMemory;
		VkImage TextureResource;
		VkDeviceMemory TextureMemory;
		VkSampler TextureSampler;
		VkImageView TextureImageView;

	};
	
}
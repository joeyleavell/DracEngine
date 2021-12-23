/**
 * @file VulkanCommon.h
 *
 * Helper functions for Vulkan.
 *
 * This file contains several functions that are helpful for Vulkan initialization.
 *
 */

#pragma once

#include "Core/Core.h"

#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"

namespace Ry
{
	struct QueueFamilies
	{
		int32 GraphicsFamily = -1;
		int32 PresentFamily = -1;
	};

	/**
	 * Allocates a new command buffer using the command pool created in the GVulkanContext.
	 *
	 * @param [out] OutBuffer	A reference to the resultant VkCommandBuffer.
	 * @param [in]  bSecondary	Whether this command buffer should be a secondary command buffer. If false, it will be a primary command buffer.
	 * @return Whether a command buffer was successfully created. If this is false, @p OutBuffer is meaningless and should not be used.
	 */
	bool AllocCmdBuffer(
		VkCommandBuffer& OutBuffer, 
		bool bSecondary = false
	);

	/**
	 * Calls vkBeginCommandBuffer with the appropriate parameters on the specified VkCommandBuffer.
	 *
	 * @param Buffer		The input command buffer
	 * @param bOneTimeUse	Whether this command buffer is intended to only be submitted once
	 */
	bool BeginCmd(
		const VkCommandBuffer& Buffer, 
		bool bOneTimeUse
	);

	/**
	 * Ends recording of a command buffer.
	 *
	 * @param Buffer	The buffer to end recording on.
	 * @return Whether the operation was successful.
	 */
	bool EndCmd(
		const VkCommandBuffer& Buffer
	);

	/**
	 * Submits the command buffer, and then immediately frees it. Useful for one-time-use command buffers.
	 *
	 * @param CmdBuffer		The VkCommandBuffer to submit and subsequently free.
	 * @return Whether the operation was successful.
	 */
	bool SubmitAndFree(
		const VkCommandBuffer& CmdBuffer
	);

	/**
	 * Attempts to find an appropriate memory type in the already chosen VkPhysicalDevice from the GVulkanContext.
	 *
	 * @param TypeFilter	The memory type bit from the physical device memory properties.
	 * @param Props			The desired properties to be supported by the memory type.
	 *
	 * @return The found memory type, or UINT32_MAX if no corresponding memory type was found.
	 */
	uint32 FindMemoryType(
		uint32 TypeFilter, 
		VkMemoryPropertyFlags Props
	);

	/**
	 * Finds the graphics and present queue families for the specified VkPhysicalDevice/VkSurfaceKHR combo.
	 *
	 * @param[out] OutQueues	Where to put the found queue family indicies.
	 * @param[in]  Device		The VkPhysicalDevice to query.
	 * @param[in]  Surface		The VkSurfaceKHR to query.
	 */
	void FindQueueFamiliesForDevice(
		QueueFamilies& OutQueues,
		VkPhysicalDevice Device, 
		VkSurfaceKHR Surface
	);

	/***
	 * Executes a primary command buffer with vkCmdCopyBufferToImage to transfer the data in SrcBuffer to DstImage.
	 * It is important to note that this function does not synchronize access to the image. For that, seek TransitionImageLayout.
	 *
	 * @param Width		The width of the image
	 * @param Height	The height of the image
	 * @param SrcBuffer	The buffer to copy from
	 * @param DstImage	The image to copy to
	 */
	void CopyBufferToImage(
		uint32 Width, 
		uint32 Height, 
		const VkBuffer& SrcBuffer, 
		const VkImage& DstImage
	);

	/**
	 * Performs an image layout transition using pipeline barriers, also ensures access to the image is synchonized.
	 *
	 * @param Image		The image who's format will be transitioned.
	 * @param OldLayout	The former layout of the image.
	 * @param NewLayout	The new layout of the image.
	 */
	void TransitionImageLayout(
		const VkImage& Image, 
		VkImageLayout OldLayout, 
		VkImageLayout NewLayout
	);

	/**
	 * Creates a new vulkan image resource and allocates the necessary device memory.
	 * For now, this function only supports creating 2D images.
	 *
	 * @param Width				The width of the image
	 * @param Height			The height of the image
	 * @param ImageFormat		The format of the image
	 * @param Tiling			The tiling to use for the image
	 * @param Usage				The manner in which the image will be used
	 * @param Props				Memory property requirements for the image device memory
	 * @param [out] OutImage	The destination to store the created image
	 * @param [out OutMemory	The destination to store the created device memory
	 * @return Whether the image and its associated resources were successfully created.
	 */
	bool CreateImage(
		uint32 Width, 
		uint32 Height, 
		VkFormat ImageFormat, 
		VkImageTiling Tiling, 
		VkImageUsageFlags Usage, 
		VkMemoryPropertyFlags Props, 
		VkImage& OutImage, 
		VkDeviceMemory& OutMemory
	);

	/**
	 * Frees the image and device memory resources associated with an image.
	 * This function does not perform any synchronization.
	 *
	 * @param Image		The image resource to destroy.
	 * @param ImageMem	The image device memory to free.
	 */
	void FreeImage(
		const VkImage& Image, 
		const VkDeviceMemory& ImageMem
	);

	/**
	 * Creates a new VkImageView for a particular VkImage.
	 *
	 * @param [out] OutView		The destination to put the new VkImageView.
	 * @param [in] ForImage		The image to create the VkImageView for.
	 * @param [in] Format		The format of the image
	 * @param [in] AspectFlags	Aspect flags for the VkImageView
	 */
	bool CreateImageView(
		VkImageView& OutView, 
		const VkImage& ForImage,
		VkFormat Format,
		VkImageAspectFlags AspectFlags
	);

	/**
	 * Given criteria, finds the best depth format for the VkPhysicalDevice selected at startup.
	 */
	bool FindOptimalDepthFormat(
		VkFormat& OutFormat, 
		const std::vector<VkFormat>& Candidates, 
		VkImageTiling Tiling, 
		VkFormatFeatureFlags Features
	);

	/**
	 * Helper function that calls <code>FindOptimalDepthFormat</code>
	 */
	bool FindDepthFormat(
		VkFormat& OutFormat
	);

	/**
	 * Returns whether a particular format has a stencil component.
	 *
	 * @param Format	The format to test
	 * @return Whether the format had a stencil component
	 */
	bool HasStencilComponent(
		VkFormat Format
	);

	/**
	 * Checks whether all validation layers specified in <code>Layers</code> are supported by the instance.
	 *
	 * @return	Whether all of the layers were supported
	 */
	bool CheckValidationLayerSupport(
		const Ry::ArrayList<Ry::String>& Layers
	);

	/**
	 * Checks whether all extensions within @p Extensions are supported by @p PhysicalDevice.
	 *
	 * @param PhysicalDev The VkPhysicalDevice to check
	 * @param Extensions The ArrayList of extensions.
	 *
	 * @return	Whether all extensions where supported by the physical device
	 */
	bool CheckDeviceExtensionSupport(
		VkPhysicalDevice PhysicalDev, 
		const Ry::ArrayList<Ry::String>& Extensions
	);

}
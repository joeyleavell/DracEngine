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

	uint32 FindMemoryType(
		uint32 TypeFilter, 
		VkMemoryPropertyFlags Props
	);
	
	void FindQueueFamiliesForDevice(
		QueueFamilies& OutQueues,
		VkPhysicalDevice Device, 
		VkSurfaceKHR Surface
	);

	void CopyBufferToImage(
		uint32 Width, 
		uint32 Height, 
		VkBuffer& SrcBuffer, 
		VkImage& DstImage
	);

	void TransitionImageLayout(
		VkImage& Image, 
		VkImageLayout OldLayout, 
		VkImageLayout NewLayout
	);

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

	bool CreateImageView(
		VkImageView& OutView, 
		const VkImage& ForImage,
		VkFormat Format,
		VkImageAspectFlags AspectFlags
	);

	bool FindOptimalDepthFormat(
		VkFormat& OutFormat, 
		const std::vector<VkFormat>& Candidates, 
		VkImageTiling Tiling, 
		VkFormatFeatureFlags Features
	);
	
	bool FindDepthFormat(
		VkFormat& OutFormat
	);
	
	bool HasStencilComponent(
		VkFormat Format
	);

	bool CheckValidationLayerSupport(
		const Ry::ArrayList<Ry::String>& Layers
	);
	
	bool CheckDeviceExtensionSupport(
		VkPhysicalDevice PhysicalDev, 
		const Ry::ArrayList<Ry::String>& Extensions
	);
	
	bool IsDeviceSuitable(
		VkPhysicalDevice PhysicalDev
	);


}
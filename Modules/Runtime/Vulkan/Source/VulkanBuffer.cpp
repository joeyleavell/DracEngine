#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "Core/Globals.h"

namespace Ry
{
	VulkanBuffer::VulkanBuffer(VkBufferUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryProps, int32 Allocated)
	{
		this->BufferSize = Allocated;
		this->BufferUsage = UsageFlags;
		this->MemoryProps = MemoryProps;
		this->bBufferCreated = false;
		
		VkBufferCreateInfo BufferInfo{};
		BufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferInfo.size = Allocated;
		BufferInfo.usage = UsageFlags;
		BufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		BufferInfo.flags = 0;

		if (vkCreateBuffer(GVulkanContext->GetLogicalDevice(), &BufferInfo, nullptr, &BufferObject) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create Vulkan vertex buffer");
		}

		vkQueueWaitIdle(GVulkanContext->GetGraphicsQueue());

		if (bBufferCreated)
		{
			vkFreeMemory(GVulkanContext->GetLogicalDevice(), BufferMemory, nullptr);
		}

		// Allocate memory on the device
		VkMemoryRequirements MemRequirements;
		vkGetBufferMemoryRequirements(GVulkanContext->GetLogicalDevice(), BufferObject, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocInfo.allocationSize = MemRequirements.size;
		AllocInfo.memoryTypeIndex = FindMemoryType(MemRequirements.memoryTypeBits, MemoryProps);

		if (vkAllocateMemory(GVulkanContext->GetLogicalDevice(), &AllocInfo, nullptr, &BufferMemory) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to allocate memory for buffer");
			return;
		}
		else
		{
			this->bBufferCreated = true;
		}

		// Associate buffer with allocated memory
		vkBindBufferMemory(GVulkanContext->GetLogicalDevice(), BufferObject, BufferMemory, 0);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());
		
		if(BufferSize > 0)
		{
			vkDestroyBuffer(GVulkanContext->GetLogicalDevice(), BufferObject, nullptr);
			vkFreeMemory(GVulkanContext->GetLogicalDevice(), BufferMemory, nullptr);
		}
	}

	template<typename T>
	void VulkanBuffer::UploadData(const T* Data, uint32 Count)
	{
		if (Count == 0)
			return;

		// TODO: This is horribly slow! Don't do this! The below vkMapMemory needs to be correctly synchronized
		//vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());

		int32 BufferSize = sizeof(T) * Count;

		void* MappedMemory = nullptr;
		// Map to CPU accessible memory
		vkMapMemory(GVulkanContext->GetLogicalDevice(), BufferMemory, 0, BufferSize, 0, &MappedMemory);
		{
			// Now copy the memory to the mapped memory section
			MemCpy(MappedMemory, BufferSize, Data, BufferSize);
		}
		vkUnmapMemory(GVulkanContext->GetLogicalDevice(), BufferMemory);
	}

	void VulkanBuffer::UploadVertData(float* Data, uint32 VertCount, uint32 VertexElementCount)
	{
		UploadData<float>(Data, VertCount * VertexElementCount);
	}

	void VulkanBuffer::UploadData(const uint32* Data, uint32 Count)
	{
		UploadData<uint32>(Data, Count);
	}

	void VulkanBuffer::UploadData(const float* Data, uint32 Count)
	{
		UploadData<float>(Data, Count);
	}

	void VulkanBuffer::UploadData(const uint8* Data, uint32 Count)
	{
		UploadData<uint8>(Data, Count);
	}

	VkBuffer& VulkanBuffer::GetBufferObject()
	{
		return BufferObject;
	}

	uint32 VulkanBuffer::GetBufferSize()
	{
		return BufferSize;
	}

}

#include "VulkanVertexArray.h"
#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "Core/Globals.h"

namespace Ry
{
	
	VulkanVertexArray::VulkanVertexArray(const VertexFormat& Format):
	VertexArray(Format)
	{
		this->Format = Format;

		// Use this buffer as transfer src, and make it visible to the host (us, the cpu)
		this->StagingVertexBuffer = nullptr;
		this->DeviceVertexBuffer = nullptr;

		this->StagingIndexBuffer = nullptr;
		this->DeviceIndexBuffer = nullptr;
	}

	void VulkanVertexArray::PushVertexData(float* Data, uint32 Vertices)
	{
		// The amount of data that's trying to be inserted
		uint32 Size = Vertices * Format.ElementCount * sizeof(float);

		if(StagingVertexBuffer && Size > StagingVertexBuffer->GetBufferSize())
		{
			delete StagingVertexBuffer;
			delete DeviceVertexBuffer;

			StagingVertexBuffer = nullptr;
			DeviceVertexBuffer = nullptr;
		}
		
		if(!StagingVertexBuffer && Size > 0)
		{
			this->StagingVertexBuffer = new VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Size);
			this->DeviceVertexBuffer = new VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Size);
		}
		
		this->VertCount = Vertices;

		if(StagingVertexBuffer && DeviceVertexBuffer)
		{
			// Map the memory to the staging buffer
			StagingVertexBuffer->UploadVertData(Data, Vertices, Format.ElementCount);

			TransferStagedToDevice(Size, StagingVertexBuffer->GetBufferObject(), DeviceVertexBuffer->GetBufferObject());
		}
	}

	void VulkanVertexArray::PushIndexData(uint32* Indices, uint32 Count)
	{
		// The amount of data that's trying to be inserted
		uint32 Size = Count * sizeof(uint32);

		if (StagingIndexBuffer && Size > StagingIndexBuffer->GetBufferSize())
		{
			delete StagingIndexBuffer;
			delete DeviceIndexBuffer;

			StagingIndexBuffer = nullptr;
			DeviceIndexBuffer = nullptr;
		}

		if (!StagingIndexBuffer && Size > 0)
		{
			this->StagingIndexBuffer = new VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Size);
			this->DeviceIndexBuffer = new VulkanBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Size);
		}

		this->IndexCount = Count;

		if (StagingIndexBuffer)
		{
			// Map the memory to the staging buffer
			StagingIndexBuffer->UploadData(Indices, Count);

			TransferStagedToDevice(Size, StagingIndexBuffer->GetBufferObject(), DeviceIndexBuffer->GetBufferObject(), DeviceIndexBuffer != nullptr);
		}

	}

	void VulkanVertexArray::DeleteArray()
	{
		vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());
		
		delete StagingVertexBuffer;
		delete StagingIndexBuffer;
		delete DeviceIndexBuffer;
		delete DeviceVertexBuffer;

		StagingIndexBuffer = nullptr;
		StagingVertexBuffer = nullptr;
		DeviceIndexBuffer = nullptr;
		DeviceVertexBuffer = nullptr;
	}

	uint32 VulkanVertexArray::GetVertexCount() const
	{
		return VertCount;
	}

	uint32 VulkanVertexArray::GetIndexCount() const
	{
		return IndexCount;
	}

	void VulkanVertexArray::TransferStagedToDevice(uint32 CopySize, VkBuffer Staged, VkBuffer Device, bool bIndexOrVertex)
	{
		// Now, create and record a command buffer for transferring data to the device-native memory
		VkCommandBufferAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocInfo.commandPool = GVulkanContext->GetCommandPool();
		AllocInfo.commandBufferCount = 1;

		VkCommandBuffer CommandBuffer;
		vkAllocateCommandBuffers(GVulkanContext->GetLogicalDevice(), &AllocInfo, &CommandBuffer);

		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // We're only using this command buffer once

		vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
		{

			VkBufferMemoryBarrier MemoryBarrier{};
			MemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			MemoryBarrier.size = CopySize;
			MemoryBarrier.buffer = Device;
			MemoryBarrier.srcAccessMask = bIndexOrVertex ? VK_ACCESS_INDEX_READ_BIT : VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
			MemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			MemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			MemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			MemoryBarrier.offset = 0;

			vkCmdPipelineBarrier(CommandBuffer, 
				VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				1,
				&MemoryBarrier,
				0,
				nullptr);
			{
				VkBufferCopy CopyRegion{};
				CopyRegion.srcOffset = 0; // Optional
				CopyRegion.dstOffset = 0; // Optional
				CopyRegion.size = CopySize;

				vkCmdCopyBuffer(CommandBuffer, Staged, Device, 1, &CopyRegion);
			}

			MemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			MemoryBarrier.dstAccessMask = bIndexOrVertex ? VK_ACCESS_INDEX_READ_BIT : VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;			MemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			vkCmdPipelineBarrier(CommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 
				0,
				0,
				nullptr,
				1,
				&MemoryBarrier,
				0,
				nullptr);

		}
		vkEndCommandBuffer(CommandBuffer);

		// Execute cmd buffer to complete transfer
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CommandBuffer;

		vkQueueSubmit(GVulkanContext->GetGraphicsQueue(), 1, &SubmitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(GVulkanContext->GetGraphicsQueue());

		// Free the command buffer since we won't be using it anymore
		vkFreeCommandBuffers(GVulkanContext->GetLogicalDevice(), GVulkanContext->GetCommandPool(), 1, &CommandBuffer);
	}


	
}

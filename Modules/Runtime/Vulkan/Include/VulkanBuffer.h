#pragma once

#include "VulkanCommon.h"
#include "VulkanGen.h"

namespace Ry
{

	class VULKAN_MODULE VulkanBuffer
	{
	public:
		VulkanBuffer(VkBufferUsageFlags UsageFlags, VkMemoryPropertyFlags MemoryProps, int32 Allocated = 0);
		~VulkanBuffer();
		
		void UploadVertData(float* Data, uint32 VertCount, uint32 VertexElementCount);
		void UploadData(const uint32* Data, uint32 Count);
		void UploadData(const float* Data, uint32 Count);
		void UploadData(const uint8* Data, uint32 Count);

		template<typename T>
		void UploadData(const T* Data, uint32 Count);

		VkBuffer& GetBufferObject();
		uint32 GetBufferSize();

	private:

		bool bBufferCreated;

		uint32 BufferSize;
		VkBufferUsageFlags BufferUsage;
		VkMemoryPropertyFlags MemoryProps;
		
		VkBuffer BufferObject;
		VkDeviceMemory BufferMemory;
		//	VkVertexInputBindingDescription BindingDesc;
		//	VkVertexInputAttributeDescription* AttributeDescriptions;
	};

	
}

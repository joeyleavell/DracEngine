#pragma once

#include "Core/Core.h"
#include "vulkan/vulkan.h"
#include "VulkanGen.h"
#include "Interface2/VertexArray2.h"

namespace Ry
{

	class VulkanBuffer;

	class VULKAN_MODULE VulkanVertexArray : public Ry::VertexArray2
	{
	public:

		// Buffer used for staging a transfer to device buffer
		VulkanBuffer* StagingVertexBuffer;
		VulkanBuffer* DeviceVertexBuffer;

		VulkanBuffer* StagingIndexBuffer;
		VulkanBuffer* DeviceIndexBuffer;

		VulkanVertexArray(const VertexFormat& Format);
		virtual ~VulkanVertexArray() {};

		virtual void PushVertexData(float* Data, uint32 Vertices);
		virtual void PushIndexData(uint32* Indices, uint32 Count);
		virtual void DeleteArray();

		virtual uint32 GetVertexCount() const;
		virtual uint32 GetIndexCount() const;

	private:

		uint32 VertCount;
		uint32 IndexCount;
		VertexFormat Format;

		/**
		 * True = index buffer, false = vertex buffer
		 * 
		 */
		void TransferStagedToDevice(uint32 CopySize, VkBuffer Staged, VkBuffer Device, bool bIndexOrVertex = true);
	};
	
}

#pragma once

#include "Core/Core.h"
#include "VulkanCommon.h"
#include "Interface/RenderCommand.h"
#include "VulkanGen.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

namespace Ry
{

	class VulkanFrameBuffer;
	class VertexArray;
	class VulkanRenderPass;
	class VulkanPipeline;

	class VULKAN_MODULE VulkanCommandBuffer : public Ry::CommandBuffer
	{
	public:

		VulkanCommandBuffer(SwapChain* SC, SecondaryCommandBufferInfo SecondaryInfo = {});
		VulkanCommandBuffer(SecondaryCommandBufferInfo SecondaryInfo = {});

		virtual ~VulkanCommandBuffer();

		void RecordBeginRenderPass(VkCommandBuffer CmdBuffer, VkFramebuffer Resource, VkExtent2D BufferExtent, Ry::RenderPass* RenderPass, bool bUseSecondary);
		void RecordEndRenderPass(VkCommandBuffer CmdBuffer);
		void RecordBindPipeline(VkCommandBuffer CmdBuffer, Pipeline* Pipeline);
		void RecordSetScissorSize(VkCommandBuffer CmdBuffer, int32 ScissorX, int32 ScissorY, uint32 ScissorWidth, uint32 ScissorHeight, int32 TargetWidth, int32 TargetHeight);
		void RecordSetViewportSize(VkCommandBuffer CmdBuffer, int32 ViewportX, int32 ViewportY, int32 ViewportWidth, int32 ViewportHeight, int32 TargetWidth, int32 TargetHeight);
		void RecordBindResourceSet(VkCommandBuffer CmdBuffer, int32 CmdBufferIndex, BindResourcesCommand* Cmd);
		void RecordDrawVertexArray(VkCommandBuffer CmdBuffer, VertexArray* VertArray, uint32 FirstVertex, uint32 Count);
		void RecordDrawVertexArrayIndexed(VkCommandBuffer CmdBuffer, VertexArray* VertArray, uint32 FirstIndex, uint32 Count);
		void RecordCommandBuffer(VkCommandBuffer CmdBuffer, int32 Index, VulkanCommandBuffer* Secondary);

		void ParseOp(VkCommandBuffer CurrentCmdBuffer, int32 CmdBufferIndex, uint8* Data, uint32& Marker);

		bool CheckDirty() override;
		void Submit() override;
		void BeginCmd() override;
		void EndCmd() override;

		void VkBeginCmd(VkCommandBuffer NewBuffer);

		void VkEndCmd(VkCommandBuffer NewBuffer);

		Ry::ArrayList<VkCommandBuffer> GeneratedBuffers;

	private:

		void ForceRecord();

		int32 SwapChainVersion;

		void RecordForIndex(int32 Index);

		bool bHasUpdatedThisFrame = false;
		Ry::ArrayList<uint32> DirtyImages;

		Ry::VulkanPipeline* BoundPipeline = nullptr;

		// Default render pass if targeting swap chain

		void CreateBuffers();
		void FreeBuffers();
		bool CreateCmdBufferResource(VkCommandBuffer& Result);
		void FreeCmdBufferResource(VkCommandBuffer Buff);
	};
	
	struct VulkanRenderPassInfo
	{
		Ry::VulkanRenderPass* RenderPass;
	
		Ry::ArrayList<Ry::VulkanCommandBuffer> CommandBuffers;
	};

	
}

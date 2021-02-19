#pragma once

#include "Core/Core.h"
#include "VulkanCommon.h"
#include "Interface2/RenderCommand.h"
#include "VulkanGen.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

namespace Ry
{

	class VulkanFrameBuffer;
	class VertexArray2;
	class VulkanRenderPass;
	class VulkanPipeline;

	// class VULKAN_MODULE VulkanPrimaryCommandBuffer
	// {
	// public:
	// 	VulkanPrimaryCommandBuffer(VulkanFrameBuffer* TargetFramebuffer);
	// 	~VulkanPrimaryCommandBuffer();
	// 	
	// 	void AddRenderPass(VulkanRenderPass* RenderPass);
	// 	void RemoveRenderPass(VulkanRenderPass* RenderPass);
	// 	
	// private:
	//
	// 	void UpdateCommandBuffer();
	//
	// 	Ry::ArrayList<VulkanRenderPass*>
	// 	
	// };

	class VULKAN_MODULE VulkanCommandBuffer2 : public Ry::RenderingCommandBuffer2
	{
	public:

		VulkanCommandBuffer2(SwapChain* SC);
		virtual ~VulkanCommandBuffer2();

		void RecordBeginRenderPass(VkCommandBuffer CmdBuffer, VulkanFrameBuffer* Target, Ry::RenderPass2* RenderPass);
		void RecordEndRenderPass(VkCommandBuffer CmdBuffer);
		void RecordBindPipeline(VkCommandBuffer CmdBuffer, Pipeline2* Pipeline);
		void RecordSetScissorSize(VkCommandBuffer CmdBuffer, int32 ScissorX, int32 ScissorY, uint32 ScissorWidth, uint32 ScissorHeight);
		void RecordSetViewportSize(VkCommandBuffer CmdBuffer, int32 ViewportX, int32 ViewportY, int32 ViewportWidth, int32 ViewportHeight);
		void RecordBindResourceSet(VkCommandBuffer CmdBuffer, int32 CmdBufferIndex, BindResourcesCommand* Cmd);
		void RecordDrawVertexArray(VkCommandBuffer CmdBuffer, VertexArray2* VertArray, uint32 FirstVertex, uint32 Count);
		void RecordDrawVertexArrayIndexed(VkCommandBuffer CmdBuffer, VertexArray2* VertArray, uint32 FirstIndex, uint32 Count);


		void ParseOp(VkCommandBuffer CurrentCmdBuffer, int32 CmdBufferIndex, VulkanFrameBuffer* Target, uint8* Data, uint32& Marker);

		void Submit() override;
		void BeginCmd() override;
		void EndCmd() override;

		void VkBeginCmd(VkCommandBuffer NewBuffer);

		void VkEndCmd(VkCommandBuffer NewBuffer);

		Ry::ArrayList<VkCommandBuffer> GeneratedBuffers;

	private:

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

	class VULKAN_MODULE VulkanCommandBuffer : public Ry::RenderingCommandBuffer
	{
	public:
	
		VulkanCommandBuffer(VulkanFrameBuffer* TargetFramebuffer);
		~VulkanCommandBuffer();
	
		bool CreateBuffer(VkCommandPool CommandPool);
		void FreeBuffer(VkCommandPool CmdPool);
	
		void BeginRecording(bool bImmediate) override;
		void EndRecording() override;
	
		void BindDescriptorSet(VkPipelineLayout PipelineLayout, VkDescriptorSet DescSet);
	
		void BeginRenderPass(Ry::RenderPass2* RenderPass) override;
		void EndRenderPass(Ry::RenderPass2* RenderPass) override;
		void BindPipeline(Ry::Pipeline2* Pipeline) override;
		void DrawVertexArray(Ry::VertexArray2* VertexArray) override;
	
		void SubmitBuffer(bool bSynchronous, VkSemaphore* WaitSemaphore = nullptr, VkSemaphore* SignalSemaphore = nullptr);
	
		VkCommandBuffer* GetBuffer();
	
	private:
	
		bool bIsImmediate;
	
		VkCommandBuffer CmdBuffer;
	
		VulkanFrameBuffer* Target;
	};
	
	struct VulkanRenderPassInfo
	{
		Ry::VulkanRenderPass* RenderPass;
	
		Ry::ArrayList<Ry::VulkanCommandBuffer> CommandBuffers;
	};

	
}

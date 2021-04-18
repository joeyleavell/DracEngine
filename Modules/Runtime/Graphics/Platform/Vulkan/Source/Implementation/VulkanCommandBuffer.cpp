#include "VulkanCommandBuffer.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "Core/Globals.h"
#include "VulkanFramebuffer.h"
#include "VulkanVertexArray.h"
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"
#include "VulkanResources.h"

namespace Ry
{

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanFrameBuffer* TargetFramebuffer)
	{
		this->Target = TargetFramebuffer;
	}
	
	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		
	}
	
	bool VulkanCommandBuffer::CreateBuffer(VkCommandPool CommandPool)
	{
		VkCommandBufferAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.commandPool = CommandPool;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocInfo.commandBufferCount = 1;// (uint32_t)CommandBuffers.size();

	
		if (vkAllocateCommandBuffers(GVulkanContext->GetLogicalDevice(), &AllocInfo, &CmdBuffer) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create command buffers!");
			return false;
		}
		else
		{
			Ry::Log->Log("Created command buffers");
			return true;
		}
	}
	
	void VulkanCommandBuffer::FreeBuffer(VkCommandPool CmdPool)
	{
		vkFreeCommandBuffers(GVulkanContext->GetLogicalDevice(), CmdPool, 1, &CmdBuffer);
	}
	
	void VulkanCommandBuffer::BeginRecording(bool bImmediate)
	{
		this->bIsImmediate = bImmediate;
		
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pInheritanceInfo = nullptr;
	
		if(bImmediate)
		{
			BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Optional
		}
		else
		{
			BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		}
	
		
		if (vkBeginCommandBuffer(CmdBuffer, &BeginInfo) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to begin recording command buffer");
		}
	}
	
	void VulkanCommandBuffer::EndRecording()
	{
		if (vkEndCommandBuffer(CmdBuffer) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to end recording command buffer");
		}
	
		if(bIsImmediate)
		{
			SubmitBuffer(bIsImmediate);
			vkFreeCommandBuffers(GVulkanContext->GetLogicalDevice(), GVulkanContext->GetCommandPool(), 1, &CmdBuffer);
		}
	}
	
	void VulkanCommandBuffer::BindDescriptorSet(VkPipelineLayout PipelineLayout, VkDescriptorSet DescSet)
	{
		// Bind the descriptor set
		vkCmdBindDescriptorSets(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayout, 0, 1, &DescSet, 0, nullptr);
	}
	
	void VulkanCommandBuffer::BeginRenderPass(Ry::RenderPass2* RenderPass)
	{
		VulkanRenderPass* VkRenderPass = dynamic_cast<VulkanRenderPass*>(RenderPass);
	
		if(!VkRenderPass)
		{
			Ry::Log->LogError("A non-Vulkan render pass was passed into BeginRenderPass()");
			return;
		}
		
		VkRenderPassBeginInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = VkRenderPass->GetRenderPass();
		RenderPassInfo.framebuffer = Target->GetResource();
		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = Target->GetFrameBufferExtent();
	
		VkClearValue ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		RenderPassInfo.clearValueCount = 1;
		RenderPassInfo.pClearValues = &ClearColor;
	
		vkCmdBeginRenderPass(CmdBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	
	void VulkanCommandBuffer::EndRenderPass(Ry::RenderPass2* RenderPass)
	{
		vkCmdEndRenderPass(CmdBuffer);
	}
	
	void VulkanCommandBuffer::BindPipeline(Ry::Pipeline2* Pipeline)
	{
		Ry::VulkanPipeline* VPipline = dynamic_cast<Ry::VulkanPipeline*>(Pipeline);
	
		if(!VPipline)
		{
			Ry::Log->LogError("Passed in a non vulkan pipeline to BindPipeline()");
			return;
		}
		
		vkCmdBindPipeline(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VPipline->GraphicsPipeline);
	}
	
	void VulkanCommandBuffer::DrawVertexArray(Ry::VertexArray2* VertexArray)
	{
		VulkanVertexArray* VkArray = dynamic_cast<VulkanVertexArray*>(VertexArray);
	
		// Bind the vertex buffer
		VkBuffer VertexBuffers[] = { VkArray->DeviceVertexBuffer->GetBufferObject() };
		VkDeviceSize Offsets[] = { 0 };
	
		vkCmdBindVertexBuffers(CmdBuffer, 0, 1, VertexBuffers, Offsets);
	
		// Draw indexed depending on if the device index buffer has been created
		if (VkArray->DeviceIndexBuffer)
		{
			vkCmdBindIndexBuffer(CmdBuffer, VkArray->DeviceIndexBuffer->GetBufferObject(), 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(CmdBuffer, VkArray->GetIndexCount(), 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(CmdBuffer, VkArray->GetVertexCount(), 1, 0, 0);
		}
	}
	
	void VulkanCommandBuffer::SubmitBuffer(bool bSynchronous, VkSemaphore* WaitSemaphore, VkSemaphore* SignalSemaphore)
	{
		// Submit the command immediately
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CmdBuffer;
	
		if(WaitSemaphore)
		{
			SubmitInfo.waitSemaphoreCount = 1;
			SubmitInfo.pWaitSemaphores = WaitSemaphore;
		}
	
		if(SignalSemaphore)
		{
			SubmitInfo.signalSemaphoreCount = 1;
			SubmitInfo.pSignalSemaphores = SignalSemaphore;
		}
	
		vkQueueSubmit(GVulkanContext->GetGraphicsQueue(), 1, &SubmitInfo, VK_NULL_HANDLE);
	
		if(bSynchronous)
		{
			vkQueueWaitIdle(GVulkanContext->GetGraphicsQueue());
		}
	
	}
	
	VkCommandBuffer* VulkanCommandBuffer::GetBuffer()
	{
		return &CmdBuffer;
	}

	VulkanCommandBuffer2::VulkanCommandBuffer2(SwapChain* SC) :
	RenderingCommandBuffer2(SC)
	{
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);

		if (!VkSC)
		{
			Ry::Log->LogError("Non vulkan swap chain used with vulkan cmd buffer");
			return;
		}

		CreateBuffers();

		this->SwapChainVersion = VkSC->GetSwapchainVersion();		
	}

	VulkanCommandBuffer2::~VulkanCommandBuffer2()
	{
		
	}

	void VulkanCommandBuffer2::CreateBuffers()
	{
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);

		// Create as many command buffers as there are frame buffer images
		for (uint32 Fb = 0; Fb < VkSC->SwapChainFramebuffers.GetSize(); Fb++)
		{
			VkCommandBuffer NewCmdBuffer;
			if (!CreateCmdBufferResource(NewCmdBuffer))
			{
				Ry::Log->LogError("Could not complete recording of Vulkan render command: failed to create command buffer");
				return;
			}

			GeneratedBuffers.Add(NewCmdBuffer);
		}
	}

	void VulkanCommandBuffer2::FreeBuffers()
	{
		for (VkCommandBuffer Buff : GeneratedBuffers)
		{
			FreeCmdBufferResource(Buff);
		}

		GeneratedBuffers.Clear();
	}

	void VulkanCommandBuffer2::RecordBeginRenderPass(VkCommandBuffer CmdBuffer, VulkanFrameBuffer* Target, Ry::RenderPass2* RenderPass)
	{
		VulkanRenderPass* VkRenderPass = dynamic_cast<VulkanRenderPass*>(RenderPass);

		if (!VkRenderPass)
		{
			Ry::Log->LogError("A non-Vulkan render pass was passed into BeginRenderPass()");
			return;
		}

		VkRenderPassBeginInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = VkRenderPass->GetRenderPass();
		RenderPassInfo.framebuffer = Target->GetResource();
		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = Target->GetFrameBufferExtent();

		Ry::ArrayList<VkClearValue> ClearValues(2);
		ClearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		ClearValues[1].depthStencil = { 1.0f, 0};
		
		RenderPassInfo.clearValueCount = 2;
		RenderPassInfo.pClearValues = ClearValues.GetData();

		vkCmdBeginRenderPass(CmdBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer2::RecordEndRenderPass(VkCommandBuffer CmdBuffer)
	{
		vkCmdEndRenderPass(CmdBuffer);
	}

	void VulkanCommandBuffer2::RecordBindPipeline(VkCommandBuffer CmdBuffer, Pipeline2* Pipeline)
	{
		Ry::VulkanPipeline* VPipeline = dynamic_cast<Ry::VulkanPipeline*>(Pipeline);

		if (!VPipeline)
		{
			Ry::Log->LogError("Passed in a non vulkan pipeline to BindPipeline()");
			return;
		}

		vkCmdBindPipeline(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, VPipeline->GraphicsPipeline);

		BoundPipeline = VPipeline;
	}

	void VulkanCommandBuffer2::RecordSetScissorSize(VkCommandBuffer CmdBuffer, int32 ScissorX, int32 ScissorY, uint32 ScissorWidth, uint32 ScissorHeight)
	{
		VkRect2D Scissor;
		Scissor.offset = { ScissorX, ScissorY };
		Scissor.extent = VkExtent2D{ScissorWidth, ScissorHeight};

		vkCmdSetScissor(CmdBuffer, 0, 1, &Scissor);
	}

	void VulkanCommandBuffer2::RecordSetViewportSize(VkCommandBuffer CmdBuffer, int32 ViewportX, int32 ViewportY, int32 ViewportWidth, int32 ViewportHeight)
	{
		VkViewport ViewportParams;
		ViewportParams.x = ViewportX;
		ViewportParams.y = Swap->GetSwapChainHeight() + ViewportY;// ViewportY;
		ViewportParams.minDepth = 0.0f;
		ViewportParams.maxDepth = 1.0f;
		ViewportParams.width = ViewportWidth;
		ViewportParams.height = -ViewportHeight;

		vkCmdSetViewport(CmdBuffer, 0, 1, &ViewportParams);
	}

	void VulkanCommandBuffer2::RecordBindResourceSet(VkCommandBuffer CmdBuffer, int32 CmdBufferIndex, BindResourcesCommand* Cmd)
	{
		Ry::ArrayList<VkDescriptorSet> DescriptorSets;
		for(int32 Set = 0; Set < Cmd->SetCount; Set++)
		{
			ResourceSet* ResSet = Cmd->SetPtrs[Set];
			
			Ry::VulkanResourceSet* VResSet = dynamic_cast<Ry::VulkanResourceSet*>(ResSet);

			if (!VResSet)
			{
				Ry::Log->LogError("Passed in a non vulkan uniform to BindUniformBuffer()");
				return;
			}

			DescriptorSets.Add(VResSet->DescriptorSets[CmdBufferIndex]);
		}

		if(BoundPipeline)
		{
			vkCmdBindDescriptorSets(CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, BoundPipeline->PipelineLayout, 0, DescriptorSets.GetSize(), DescriptorSets.GetData(), 0, nullptr);
		}
		else
		{
			Ry::Log->LogError("Must bind pipeline before binding uniform descriptor sets");
		}
	}

	void VulkanCommandBuffer2::RecordDrawVertexArray(VkCommandBuffer CmdBuffer, VertexArray2* VertArray, uint32 FirstVertex, uint32 Count)
	{
		VulkanVertexArray* VkArray = dynamic_cast<VulkanVertexArray*>(VertArray);

		// Bind the vertex buffer
		VkBuffer VertexBuffers[] = { VkArray->DeviceVertexBuffer->GetBufferObject() };
		VkDeviceSize Offsets[] = { 0 };
		int32 VertexCount = Count > 0 ? Count : VkArray->GetVertexCount(); // Draw all elements if count is zero

		vkCmdBindVertexBuffers(CmdBuffer, 0, 1, VertexBuffers, Offsets);
		vkCmdDraw(CmdBuffer, VertexCount, 1, FirstVertex, 0);
	}

	void VulkanCommandBuffer2::RecordDrawVertexArrayIndexed(VkCommandBuffer CmdBuffer, VertexArray2* VertArray, 
		uint32 FirstIndex, uint32 Count)
	{
		VulkanVertexArray* VkArray = dynamic_cast<VulkanVertexArray*>(VertArray);

		if (!VkArray->DeviceIndexBuffer)
		{
			Ry::Log->LogError("Vulkan vertex buffer had no index buffer in DrawVertexArrayIndexed()");
			return;
		}

		if(VkArray->GetVertexCount() > 0 && VkArray->GetIndexCount() > 0)
		{
			VkBuffer VertexBuffers[] = { VkArray->DeviceVertexBuffer->GetBufferObject() };
			VkDeviceSize Offsets[] = { 0 };

			vkCmdBindVertexBuffers(CmdBuffer, 0, 1, VertexBuffers, Offsets);
			{
				vkCmdBindIndexBuffer(CmdBuffer, VkArray->DeviceIndexBuffer->GetBufferObject(), 0, VK_INDEX_TYPE_UINT32);
				{
					int32 IndexCount = Count > 0 ? Count : VkArray->GetIndexCount(); // Draw all elements if count is zero

					vkCmdDrawIndexed(CmdBuffer, IndexCount, 1, FirstIndex, 0, 0);
				}
			}
		}

	}

	void VulkanCommandBuffer2::ParseOp(VkCommandBuffer CurrentCmdBuffer, int32 CmdBufferIndex, VulkanFrameBuffer* Target, uint8* Data, uint32& Marker)
	{
		uint8 NextOpCode = Data[Marker];

		Marker++;

		if (NextOpCode == OP_BEGIN_RENDER_PASS)
		{
			BeginRenderPassCommand* BeginCmd = ExtractToken<BeginRenderPassCommand>(Marker, Data);
			RecordBeginRenderPass(CurrentCmdBuffer, Target, BeginCmd->RenderPass);
		}

		if (NextOpCode == OP_END_RENDER_PASS)
		{
			// No data for this command
			RecordEndRenderPass(CurrentCmdBuffer);
		}

		if (NextOpCode == OP_BIND_PIPELINE)
		{
			BindPipelineCommand* Cmd = ExtractToken<BindPipelineCommand>(Marker, Data);
			RecordBindPipeline(CurrentCmdBuffer, Cmd->Pipeline);
		}

		if (NextOpCode == OP_BIND_RESOURCE_SET)
		{
			BindResourcesCommand* Cmd = ExtractToken<BindResourcesCommand>(Marker, Data);
			RecordBindResourceSet(CurrentCmdBuffer, CmdBufferIndex, Cmd);
		}

		if (NextOpCode == OP_SET_VIEWPORT_SIZE)
		{
			SetViewportSizeCmd* Cmd = ExtractToken<SetViewportSizeCmd>(Marker, Data);
			RecordSetViewportSize(CurrentCmdBuffer, Cmd->ViewportX, Cmd->ViewportY, Cmd->ViewportWidth, Cmd->ViewportHeight);
		}

		if (NextOpCode == OP_SET_SCISSOR_SIZE)
		{
			SetViewportScissorCmd* Cmd = ExtractToken<SetViewportScissorCmd>(Marker, Data);
			RecordSetScissorSize(CurrentCmdBuffer, Cmd->ScissorX, Cmd->ScissorY, Cmd->ScissorWidth, Cmd->ScissorHeight);
		}

		if (NextOpCode == OP_DRAW_VERTEX_ARRAY)
		{
			DrawVertexArrayCommand* Cmd = ExtractToken<DrawVertexArrayCommand>(Marker, Data);
			RecordDrawVertexArray(CurrentCmdBuffer, Cmd->VertexArray, Cmd->FirstVertex, Cmd->VertexCount);
		}

		if (NextOpCode == OP_DRAW_VERTEX_ARRAY_INDEXED)
		{
			DrawVertexArrayIndexedCommand* Cmd = ExtractToken<DrawVertexArrayIndexedCommand>(Marker, Data);
			RecordDrawVertexArrayIndexed(CurrentCmdBuffer, Cmd->VertexArray, Cmd->FirstIndex, Cmd->IndexCount);
		}

	}

	void VulkanCommandBuffer2::Submit()
	{		
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);

		if(SwapChainVersion != VkSC->GetSwapchainVersion())
		{
			vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());

			// Recreate command buffers (swap chain image count could have changed for all we know)
			FreeBuffers();
			CreateBuffers();

			for(uint32 Fb = 0; Fb < VkSC->SwapChainFramebuffers.GetSize(); Fb++)
			{
				RecordForIndex(Fb);
			}
			
			SwapChainVersion = VkSC->GetSwapchainVersion();
		}
		else if (!bHasUpdatedThisFrame)
		{
			int32 FrameIndex = VkSC->GetCurrentImageIndex();

			if(DirtyImages.Contains(FrameIndex))
			{
				// Rebuild this buffer
				RecordForIndex(FrameIndex);
				
				DirtyImages.Remove(FrameIndex);
			}
			// Need to rebuild old
		}

		VkSC->SubmitBuffer(GeneratedBuffers[VkSC->GetCurrentImageIndex()]);

		bHasUpdatedThisFrame = false;
	}

	void VulkanCommandBuffer2::BeginCmd()
	{
		Reset(); // Always implicitly reset command buffer
	}

	void VulkanCommandBuffer2::EndCmd()
	{
		if (!Swap)
		{
			Ry::Log->LogError("Only command buffers targeting swap chain framebuffers are supported for Vulkan the moment");
			return;
		}

		// Record a primary command buffer for each framebuffer within the swap chain
		if (Swap)
		{
			VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);

			// First check if we're recording this during a frame
			int32 CurrentImageIndex = VkSC->GetCurrentImageIndex();

			Ry::ArrayList<uint32> BuffersToRecord;

			if(CurrentImageIndex < 0)
			{
				// Record all buffers, have to perform device wait to ensure no operations
				for(uint32 Fb = 0; Fb < VkSC->SwapChainFramebuffers.GetSize(); Fb++)
				{
					BuffersToRecord.Add(Fb);
				}
			}
			else
			{
				// Synchronization is guaranteed, record to the current command buffer and update next frames later if needed
				BuffersToRecord.Add(CurrentImageIndex);

				// Dirty the images that are not the current one
				for (int32 Fb = 0; Fb < VkSC->SwapChainFramebuffers.GetSize(); Fb++)
				{
					if(Fb != CurrentImageIndex && !DirtyImages.Contains(Fb)) // todo: this should be an efficient set
					{
						DirtyImages.Add(Fb);
					}
				}

			}

			for(uint32 Buffer = 0; Buffer < BuffersToRecord.GetSize(); Buffer++)
			{
				RecordForIndex(BuffersToRecord[Buffer]);
			}

		}

		if(bImmediate)
		{
			// Submit
		}

		if(bOneTimeUse)
		{
			// Free command
			FreeBuffers();
		}

		bHasUpdatedThisFrame = true;
		
	}

	void VulkanCommandBuffer2::VkBeginCmd(VkCommandBuffer NewBuffer)
	{
		//vkResetCommandBuffer(NewBuffer, 0);
		
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pInheritanceInfo = nullptr;

		if (bOneTimeUse)
		{
			BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		}
		else
		{
			BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		}

		if (vkBeginCommandBuffer(NewBuffer, &BeginInfo) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to begin recording command buffer");
		}
	}

	void VulkanCommandBuffer2::VkEndCmd(VkCommandBuffer NewBuffer)
	{
		if (vkEndCommandBuffer(NewBuffer) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to end recording command buffer");
		}

		//SubmitBuffer(bIsImmediate);
		//vkFreeCommandBuffers(GVulkanContext->GetLogicalDevice(), GVulkanContext->CommandPool, 1, &CmdBuffer);


		// if (bIsImmediate)
		// {
		// 	SubmitBuffer(bIsImmediate);
		// 	vkFreeCommandBuffers(GVulkanContext->GetLogicalDevice(), GVulkanContext->CommandPool, 1, &CmdBuffer);
		// }
	}

	void VulkanCommandBuffer2::RecordForIndex(int32 Index)
	{
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);

		VkCommandBuffer BufferToRecord = GeneratedBuffers[Index];
		VulkanFrameBuffer* Framebuffer = VkSC->SwapChainFramebuffers[Index];

		VkBeginCmd(BufferToRecord);
		{
			// Begin readback of the recorded command
			uint32 ReadBack = 0;

			while (ReadBack < Marker)
			{
				// Parse op will directly change marker val
				ParseOp(BufferToRecord, Index, Framebuffer, CmdBuffer, ReadBack);
			}
		}
		VkEndCmd(BufferToRecord);
	}

	bool VulkanCommandBuffer2::CreateCmdBufferResource(VkCommandBuffer& Result)
	{
		VkCommandBufferAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.commandPool = GVulkanContext->GetCommandPool();
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocInfo.commandBufferCount = 1;// (uint32_t)CommandBuffers.size();

		if (vkAllocateCommandBuffers(GVulkanContext->GetLogicalDevice(), &AllocInfo, &Result) == VK_SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void VulkanCommandBuffer2::FreeCmdBufferResource(VkCommandBuffer Buff)
	{
		vkFreeCommandBuffers(GVulkanContext->GetLogicalDevice(), GVulkanContext->GetCommandPool(), 1, &Buff);
	}


}

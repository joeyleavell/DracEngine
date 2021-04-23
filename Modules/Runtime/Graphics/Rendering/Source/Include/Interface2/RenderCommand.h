#pragma once

#include "Core/Core.h"
#include "Core/Globals.h"
#include "RenderingGen.h"
#include "SwapChain.h"

namespace Ry
{

	constexpr uint32 MAX_COMMAND_BUFFER_SIZE = 1024 * 10;

	// Use a single byte for the render opcode
	constexpr uint8 OP_NONE = 0;
	constexpr uint8 OP_DRAW_VERTEX_ARRAY = 1;
	constexpr uint8 OP_DRAW_VERTEX_ARRAY_INDEXED = 2;
	constexpr uint8 OP_BEGIN_RENDER_PASS = 3;
	constexpr uint8 OP_END_RENDER_PASS = 4;
	constexpr uint8 OP_BIND_PIPELINE = 5;
	constexpr uint8 OP_SET_VIEWPORT_SIZE = 6;
	constexpr uint8 OP_SET_SCISSOR_SIZE = 7;
	constexpr uint8 OP_BIND_RESOURCE_SET = 8;
	constexpr uint8 OP_COMMAND_BUFFER = 9;

	class Pipeline2;
	class VertexArray2;
	class RenderPass2;
	class SwapChain;
	class Framebuffer2;
	class ResourceSet;

	struct RENDERING_MODULE SetViewportSizeCmd
	{
		int32 ViewportX;
		int32 ViewportY;
		int32 ViewportWidth;
		int32 ViewportHeight;
	};

	struct RENDERING_MODULE SetViewportScissorCmd
	{
		int32 ScissorX;
		int32 ScissorY;
		uint32 ScissorWidth;
		uint32 ScissorHeight;
	};
	
	struct RENDERING_MODULE BindPipelineCommand
	{
		Ry::Pipeline2* Pipeline;
	};

	struct RENDERING_MODULE BindResourcesCommand
	{
		int32 SetCount = 0;
		ResourceSet** SetPtrs = nullptr;
	};

	struct RENDERING_MODULE DrawVertexArrayCommand
	{
		Ry::VertexArray2* VertexArray;
		int32 FirstVertex = 0;
		int32 VertexCount = 0;
	};

	struct RENDERING_MODULE DrawVertexArrayIndexedCommand
	{
		Ry::VertexArray2* VertexArray;
		int32 FirstIndex = 0;
		int32 IndexCount = 0;
	};

	struct RENDERING_MODULE CommandBufferCommand
	{
		Ry::RenderingCommandBuffer2* CmdBuffer = nullptr;
	};


	struct RENDERING_MODULE BeginRenderPassCommand
	{
		Ry::RenderPass2* RenderPass;
		bool bUseSecondary;
	};

	class RENDERING_MODULE RenderCommand
	{
	public:
		RenderCommand();
		~RenderCommand();

		virtual void Execute() = 0;
		
	};

	struct SecondaryCommandBufferInfo
	{
		bool bSecondary = false;
		RenderPass2* ParentRenderPass = nullptr;
	};

	class RENDERING_MODULE RenderingCommandBuffer2
	{
	public:

		/**
		 * Tells the command buffer to generate a buffer compatible with the swap chain frame buffer.
		 */
		RenderingCommandBuffer2(SwapChain* SC, SecondaryCommandBufferInfo SecondaryInfo = {}) :
		Marker(0)
		{
			this->Swap = SC;
			this->SecondaryInfo = SecondaryInfo;
			bDirty = false;
			bOneTimeUse = false;
			bImmediate = false;
		}

		// todo: don't force early bind here, do so when beginning render pass
		RenderingCommandBuffer2(Framebuffer2* Fb):
		Marker(0)
		{
			bDirty = false;
			bOneTimeUse = true;
			bImmediate = false;
		}

		bool IsOneTimeUse()
		{
			return bOneTimeUse;
		}

		void SetOneTimeUse(bool bUseOnce)
		{
			this->bOneTimeUse = bUseOnce;
		}

		void UpdateParentRenderPass(RenderPass2* RenderPass)
		{
			this->SecondaryInfo.ParentRenderPass = RenderPass;
		}

		template<typename T>
		T* ExtractToken(uint32& Marker, uint8* Data)
		{
			// Extract the command from the buffer
			T* ExtractedCmd = reinterpret_cast<T*>(Data + Marker);

			// Progress marker forward by command size
			Marker += sizeof(*ExtractedCmd);

			// return result
			return ExtractedCmd;
		}

		virtual void BeginRenderPass(bool bUseSecondary = false)
		{
			if(!Swap)
			{
				Ry::Log->LogError("Must have swap chain as target to specify no render pass");
				return;
			}
			
			BeginRenderPassCommand Cmd{ Swap->GetDefaultRenderPass(), bUseSecondary };
			PushCmdData(&Cmd, sizeof(Cmd), OP_BEGIN_RENDER_PASS);
		}
		
		virtual void BeginRenderPass(Ry::RenderPass2* RenderPass, bool bUseSecondary)
		{
			BeginRenderPassCommand Cmd{ RenderPass, bUseSecondary};
			PushCmdData(&Cmd, sizeof(Cmd), OP_BEGIN_RENDER_PASS);
		}
		
		virtual void EndRenderPass()
		{
			PushCmdDataNoData(OP_END_RENDER_PASS);
		}

		virtual void SetViewportSize(int32 ViewportX, int32 ViewportY, int32 ViewportWidth, int32 ViewportHeight)
		{
			SetViewportSizeCmd Cmd{ ViewportX, ViewportY, ViewportWidth, ViewportHeight};
			PushCmdData(&Cmd, sizeof(Cmd), OP_SET_VIEWPORT_SIZE);
		}

		virtual void SetScissorSize(int32 ScissorX, int32 ScissorY, uint32 ScissorWidth, uint32 ScissorHeight)
		{
			SetViewportScissorCmd Cmd{ ScissorX, ScissorY, ScissorWidth, ScissorHeight };
			PushCmdData(&Cmd, sizeof(Cmd), OP_SET_SCISSOR_SIZE);
		}

		virtual void BindPipeline(Ry::Pipeline2* Pipeline)
		{
			BindPipelineCommand Cmd{ Pipeline };
			PushCmdData(&Cmd, sizeof(Cmd), OP_BIND_PIPELINE);
		}

		virtual void BindResources(ResourceSet** SetPtrs, int32 SetCount)
		{
			BindResourcesCommand Cmd;
			Cmd.SetPtrs = SetPtrs;
			Cmd.SetCount = SetCount;
			
			PushCmdData(&Cmd, sizeof(Cmd), OP_BIND_RESOURCE_SET);
		}

		virtual void DrawVertexArray(Ry::VertexArray2* VertexArray, int32 FirstVertex = 0, int32 Count = 0)
		{
			DrawVertexArrayCommand Cmd{ VertexArray, FirstVertex, Count};
			PushCmdData(&Cmd, sizeof(Cmd), OP_DRAW_VERTEX_ARRAY);
		}

		virtual void DrawVertexArrayIndexed(Ry::VertexArray2* VertexArray, int32 FirstIndex = 0, int32 Count = 0)
		{
			DrawVertexArrayIndexedCommand Cmd{ VertexArray, FirstIndex, Count};
			PushCmdData(&Cmd, sizeof(Cmd), OP_DRAW_VERTEX_ARRAY_INDEXED);
		}

		virtual void CommandBuffer(Ry::RenderingCommandBuffer2* CmdBuffer)
		{
			CommandBufferCommand Cmd{ CmdBuffer };
			PushCmdData(&Cmd, sizeof(Cmd), OP_COMMAND_BUFFER);

			SecondaryBuffers.Add(CmdBuffer);
		}

		virtual void Reset()
		{
			Marker = 0;
			bDirty = true;

			SecondaryBuffers.Clear();
		}

		virtual bool CheckDirty() = 0;
		virtual void Submit() = 0;
		virtual void BeginCmd() = 0;
		virtual void EndCmd() = 0;

	protected:

		Ry::ArrayList<RenderingCommandBuffer2*> SecondaryBuffers;

		SecondaryCommandBufferInfo SecondaryInfo;
		
		bool bDirty;
		SwapChain* Swap;

		void PushCmdDataNoData(uint8 OpCode)
		{
			if (Marker + 1 /* OpCode byte */ >= MAX_COMMAND_BUFFER_SIZE)
			{
				Ry::Log->LogError("Cmd exceeded buffer size");
				return;
			}

			// Set the command OpCode
			CmdBuffer[Marker] = OpCode;
			Marker++;

			bDirty = true;
		}
		
		void PushCmdData(void* Data, int32 DataSize, uint8 OpCode)
		{
			if (Marker + DataSize + 1 /* OpCode byte */ >= MAX_COMMAND_BUFFER_SIZE)
			{
				Ry::Log->LogError("Cmd exceeded buffer size");
				return;
			}

			// Set the command OpCode
			CmdBuffer[Marker] = OpCode;
			Marker++;

			MemCpy(CmdBuffer + Marker, MAX_COMMAND_BUFFER_SIZE - Marker, Data, DataSize);

			Marker += DataSize;

			bDirty = false;
		}
		
		uint8 CmdBuffer [MAX_COMMAND_BUFFER_SIZE];
		uint32 Marker;

		bool bOneTimeUse;
		bool bImmediate;


	};

	class RENDERING_MODULE RenderingCommandBuffer
	{
	public:

		virtual void BeginRecording(bool bImmediate) = 0;
		virtual void EndRecording() = 0;

		virtual void BeginRenderPass(Ry::RenderPass2* Pipeline) = 0;
		virtual void EndRenderPass(Ry::RenderPass2* Pipeline) = 0;

		virtual void BindPipeline(Ry::Pipeline2* Pipeline) = 0;

		virtual void DrawVertexArray(Ry::VertexArray2* VertexArray) = 0;

		
	};
	
}

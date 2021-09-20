#pragma once

#include "Interface/RenderCommand.h"
#include "OpenGLGen.h"

namespace Ry
{
	class GLState;

	class OPENGL_MODULE GLCommandBuffer : public Ry::CommandBuffer
	{

	public:

		GLCommandBuffer(SwapChain* SC, SecondaryCommandBufferInfo SecondaryInfo = {}) :
		CommandBuffer(SC, SecondaryInfo)
		{
			BoundState = nullptr;
		}


		bool CheckDirty() override;
		void Submit() override;
		void BeginCmd() override;
		void EndCmd() override;

	private:

		void ParseOp(uint8* Data, uint32& Marker);

		void GLBeginRenderPass(BeginRenderPassCommand* Cmd);
		void GLEndRenderPass();
		void GLSetViewportSize(SetViewportSizeCmd* Cmd);
		void GLSetScissorSize(SetViewportScissorCmd* Cmd);
		void GLBindResources(BindResourcesCommand* Cmd);
		void GLDrawVertexArray(DrawVertexArrayCommand* Cmd);
		void GLDrawVertexArrayIndexed(DrawVertexArrayIndexedCommand* Cmd);
		void GLBindState(BindPipelineCommand* Cmd);
		void GLCmdBufferCommand(CommandBufferCommand* Cmd);

		GLState* BoundState;

		//int32 GLCommandBuffer::PrimitiveToGL(Primitive prim) const;
		
	};
	
}
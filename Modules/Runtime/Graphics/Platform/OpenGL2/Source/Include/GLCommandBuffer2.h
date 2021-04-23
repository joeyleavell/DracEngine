#pragma once

#include "Interface2/RenderCommand.h"
#include "OpenGL2Gen.h"

namespace Ry
{
	class GLState;

	class OPENGL2_MODULE GLCommandBuffer2 : public Ry::RenderingCommandBuffer2
	{

	public:

		GLCommandBuffer2(SwapChain* SC, SecondaryCommandBufferInfo SecondaryInfo = {}) :
		RenderingCommandBuffer2(SC, SecondaryInfo)
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

		GLState* BoundState;

		//int32 GLCommandBuffer2::PrimitiveToGL(Primitive prim) const;
		
	};
	
}
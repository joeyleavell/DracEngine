#include "GLCommandBuffer2.h"
#include "glew.h"
#include "GLShader2.h"
#include "Interface2/Pipeline.h"
#include "GLVertexArray2.h"
#include "GLResources2.h"
#include "GLRenderingState2.h"

namespace Ry
{
	bool GLCommandBuffer2::CheckDirty()
	{
		return false;
	}

	void GLCommandBuffer2::Submit()
	{
		// Begin readback of the recorded command
		uint32 ReadBack = 0;

		while (ReadBack < Marker)
		{
			// Parse op will directly change marker val
			ParseOp(CmdBuffer, ReadBack);
		}
	}

	void GLCommandBuffer2::BeginCmd()
	{

	}

	void GLCommandBuffer2::EndCmd()
	{

	}

	void GLCommandBuffer2::ParseOp(uint8* Data, uint32& Marker)
	{
		uint8 NextOpCode = Data[Marker];

		Marker++;

		if (NextOpCode == OP_BEGIN_RENDER_PASS)
		{
			BeginRenderPassCommand* BeginCmd = ExtractToken<BeginRenderPassCommand>(Marker, Data);
			GLBeginRenderPass(BeginCmd);
		}

		if (NextOpCode == OP_END_RENDER_PASS)
		{
			// No data for this command
			GLEndRenderPass();
		}

		if (NextOpCode == OP_BIND_PIPELINE)
		{
			BindPipelineCommand* Cmd = ExtractToken<BindPipelineCommand>(Marker, Data);
			GLBindState(Cmd);
		}

		if (NextOpCode == OP_BIND_RESOURCE_SET)
		{
			BindResourcesCommand* Cmd = ExtractToken<BindResourcesCommand>(Marker, Data);
			GLBindResources(Cmd);
		}

		if (NextOpCode == OP_SET_VIEWPORT_SIZE)
		{
			SetViewportSizeCmd* Cmd = ExtractToken<SetViewportSizeCmd>(Marker, Data);
			GLSetViewportSize(Cmd);
		}

		if (NextOpCode == OP_SET_SCISSOR_SIZE)
		{
			SetViewportScissorCmd* Cmd = ExtractToken<SetViewportScissorCmd>(Marker, Data);
			GLSetScissorSize(Cmd);
		}

		if (NextOpCode == OP_DRAW_VERTEX_ARRAY)
		{
			DrawVertexArrayCommand* Cmd = ExtractToken<DrawVertexArrayCommand>(Marker, Data);
			GLDrawVertexArray(Cmd);
		}

		if (NextOpCode == OP_DRAW_VERTEX_ARRAY_INDEXED)
		{
			DrawVertexArrayIndexedCommand* Cmd = ExtractToken<DrawVertexArrayIndexedCommand>(Marker, Data);
			GLDrawVertexArrayIndexed(Cmd);
		}

		if (NextOpCode == OP_COMMAND_BUFFER)
		{
			CommandBufferCommand* Cmd = ExtractToken<CommandBufferCommand>(Marker, Data);
			GLCmdBufferCommand(Cmd);
		}

	}

	void GLCommandBuffer2::GLBeginRenderPass(Ry::BeginRenderPassCommand* Cmd)
	{
		// todo: only change state here if we switch render passes
		
		// Bind default framebuffer (for now)
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void GLCommandBuffer2::GLEndRenderPass()
	{
		
	}

	void GLCommandBuffer2::GLSetViewportSize(SetViewportSizeCmd* Cmd)
	{
		glViewport(Cmd->ViewportX, Cmd->ViewportY, Cmd->ViewportWidth, Cmd->ViewportHeight);
	}

	void GLCommandBuffer2::GLSetScissorSize(SetViewportScissorCmd* Cmd)
	{
		glScissor(Cmd->ScissorX, Cmd->ScissorY, Cmd->ScissorWidth, Cmd->ScissorHeight);
	}

	void GLCommandBuffer2::GLBindResources(BindResourcesCommand* Cmd)
	{
		// Bind the uniforms to the shader
		GLShader2* BoundShader = dynamic_cast<GLShader2*>(BoundState->GetCreateInfo().PipelineShader);

		// Flush resource data
		for(int32 ResSet = 0; ResSet < Cmd->SetCount; ResSet++)
		{
			GLResources2* Resources = dynamic_cast<GLResources2*>(Cmd->SetPtrs[ResSet]);

			// Flush the data now
			Resources->FlushBuffer(0);

			for(const ConstantBuffer* Buff : Resources->GetInfo()->ConstantBuffers)
			{
				Ry::String Name = "type_" + Buff->Name;

				GLuint BlockIndex = glGetUniformBlockIndex(BoundShader->GetProgramHandle(), *Name);
				glUniformBlockBinding(BoundShader->GetProgramHandle(), BlockIndex, Buff->Index);
			}

			for (const TextureBinding* Texture : Resources->GetInfo()->TextureBindings)
			{
				Ry::MappedTexture* MappedTexture = Resources->GetMappedTexture(Texture->Name);
				
				glActiveTexture(GL_TEXTURE0 + MappedTexture->Slot);
				glBindTexture(MappedTexture->Target, MappedTexture->Texture);		
				//glActiveTexture(GL_TEXTURE0); // Switch back to default texture unit
			}
		}

	}

	void GLCommandBuffer2::GLDrawVertexArray(DrawVertexArrayCommand* Cmd)
	{
		const GLVertexArray2* VertArray = dynamic_cast<const GLVertexArray2*>(Cmd->VertexArray);

		glBindVertexArray(VertArray->GetVaoHandle());
		glDrawArrays(GL_TRIANGLES, Cmd->FirstVertex, Cmd->VertexCount);
		glBindVertexArray(0);

	}

	void GLCommandBuffer2::GLDrawVertexArrayIndexed(DrawVertexArrayIndexedCommand* Cmd)
	{
		const GLVertexArray2* VertArray = dynamic_cast<const GLVertexArray2*>(Cmd->VertexArray);

		glBindVertexArray(VertArray->GetVaoHandle());
		glDrawElements(GL_TRIANGLES, Cmd->IndexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(Cmd->FirstIndex * sizeof(int32)));
		glBindVertexArray(0);
	}

	void GLCommandBuffer2::GLBindState(BindPipelineCommand* Cmd)
	{
		GLState* State = dynamic_cast<GLState*>(Cmd->Pipeline);

		// Todo: only change this state if needed
		// This is the opengl state block - state changes should not occur outside of here

		//glEnable(GL_SCISSOR_TEST);
		const PipelineCreateInfo& StateInfo = State->GetCreateInfo();

		if(StateInfo.Depth.bEnableDepthTest)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if(StateInfo.Blend.bEnabled)
		{
			static Ry::ArrayList<GLuint> MappedBlendFuncs = {GL_SRC_ALPHA,
				GL_DST_ALPHA,
				GL_ONE_MINUS_SRC_ALPHA,
				GL_ONE_MINUS_DST_ALPHA,
				GL_ONE, GL_ZERO
			};

			static Ry::ArrayList<GLuint> MappedBlendOps = { GL_FUNC_ADD
			};
			
			glEnable(GL_BLEND);

			GLuint SrcFactor = MappedBlendFuncs[static_cast<uint32>(StateInfo.Blend.SrcFactor)];
			GLuint DstFactor = MappedBlendFuncs[static_cast<uint32>(StateInfo.Blend.DstFactor)];
			GLuint SrcAlFactor = MappedBlendFuncs[static_cast<uint32>(StateInfo.Blend.SrcAlphaFactor)];
			GLuint DstAlFactor = MappedBlendFuncs[static_cast<uint32>(StateInfo.Blend.DstAlphaFactor)];

			glBlendFuncSeparate(SrcFactor, DstFactor, SrcAlFactor, DstAlFactor);
			glBlendEquation(MappedBlendFuncs[static_cast<uint32>(StateInfo.Blend.Op)]);			
		}
		else
		{
			glDisable(GL_BLEND);
		}
		
		glEnable(GL_TEXTURE_2D);
		//glEnable(GL_FRAMEBUFFER_SRGB);
		
		// Bind OpenGL shader
		const GLShader2* Shader = dynamic_cast<const GLShader2*>(Cmd->Pipeline->GetCreateInfo().PipelineShader);
		glUseProgram(Shader->GetProgramHandle());

		// Todo: Set blend parameters

		this->BoundState = dynamic_cast<GLState*>(Cmd->Pipeline);
	}

	void GLCommandBuffer2::GLCmdBufferCommand(CommandBufferCommand* Cmd)
	{
		GLCommandBuffer2* Secondary = dynamic_cast<GLCommandBuffer2*>(Cmd->CmdBuffer);

		CORE_ASSERT(Secondary != nullptr);

		Secondary->Submit();
	}

	// Todo: how do we want to handle setting render primitives across APIs?
	// OpenGL does this per draw whereas Vulkan does this as part of the pipeline
	// Will probably need to put this as part of the pipeline with OpenGL as well
	// 
	// int32 GLCommandBuffer2::PrimitiveToGL(Primitive prim) const
	// {
	// 	int32 gl_primitive = GL_TRIANGLES;
	//
	// 	switch (prim)
	// 	{
	// 	case Primitive::TRIANGLE:
	// 		gl_primitive = GL_TRIANGLES;
	// 		break;
	// 	case Primitive::LINE:
	// 		gl_primitive = GL_LINES;
	// 		break;
	// 	}
	//
	// 	return gl_primitive;
	// }
	
}

#include "GLCommandBuffer.h"
#include "glew.h"
#include "GLShader.h"
#include "Interface/Pipeline.h"
#include "GLVertexArray.h"
#include "GLResources.h"
#include "GLRenderingState.h"
#include "GLFrameBuffer.h"

namespace Ry
{
	bool GLCommandBuffer::CheckDirty()
	{
		return false;
	}

	void GLCommandBuffer::Submit()
	{

		// Begin readback of the recorded command
		uint32 ReadBack = 0;

		while (ReadBack < Marker)
		{
			// Parse op will directly change marker val
			ParseOp(CmdBuffer, ReadBack);
		}

	}

	void GLCommandBuffer::BeginCmd()
	{

	}

	void GLCommandBuffer::EndCmd()
	{

	}

	void GLCommandBuffer::ParseOp(uint8* Data, uint32& Marker)
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

	void GLCommandBuffer::GLBeginRenderPass(Ry::BeginRenderPassCommand* Cmd)
	{
		// Bind default framebuffer (for now)
		if(Cmd->SourceBuffer)
		{
			// Bind default framebuffer
			if(Ry::GLFrameBuffer* GLFbo = dynamic_cast<Ry::GLFrameBuffer*>(Cmd->SourceBuffer))
			{
				if(GLFbo->IsDefaultAlias())
				{
					// This framebuffer is just an alias for the default framebuffer object
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}
				else
				{
					// Bind the framebuffer's handle
					glBindFramebuffer(GL_FRAMEBUFFER, GLFbo->GetHandle());
				}
			}
			else
			{
				Ry::Log->LogError("GLCommandBuffer::GLBeginRenderPass: Specified framebuffer was not an OpenGL framebuffer!");
			}
		}
		else
		{
			// No source buffer specified, must be default buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Force scissor test off for clearing screen
		glDisable(GL_SCISSOR_TEST);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void GLCommandBuffer::GLEndRenderPass()
	{
	}

	void GLCommandBuffer::GLSetViewportSize(SetViewportSizeCmd* Cmd)
	{
		glViewport(Cmd->ViewportX, Cmd->ViewportY, Cmd->ViewportWidth, Cmd->ViewportHeight);
	}

	void GLCommandBuffer::GLSetScissorSize(SetViewportScissorCmd* Cmd)
	{
		glScissor(Cmd->ScissorX, Cmd->ScissorY, Cmd->ScissorWidth, Cmd->ScissorHeight);
	}

	void GLCommandBuffer::GLBindResources(BindResourcesCommand* Cmd)
	{
		// Bind the uniforms to the shader
		GLShader* BoundShader = dynamic_cast<GLShader*>(BoundState->GetCreateInfo().PipelineShader);

		// Flush resource data
		for(int32 ResSet = 0; ResSet < Cmd->SetCount; ResSet++)
		{
			GLResources* Resources = dynamic_cast<GLResources*>(Cmd->SetPtrs[ResSet]);

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

	void GLCommandBuffer::GLDrawVertexArray(DrawVertexArrayCommand* Cmd)
	{
		const GLVertexArray* VertArray = dynamic_cast<const GLVertexArray*>(Cmd->VertexArray);

		if(Cmd->VertexCount > 0)
		{
			glBindVertexArray(VertArray->GetVaoHandle());
			glDrawArrays(GL_TRIANGLES, Cmd->FirstVertex, Cmd->VertexCount);
			glBindVertexArray(0);
		}

	}

	void GLCommandBuffer::GLDrawVertexArrayIndexed(DrawVertexArrayIndexedCommand* Cmd)
	{
		const GLVertexArray* VertArray = dynamic_cast<const GLVertexArray*>(Cmd->VertexArray);

		if(Cmd->IndexCount > 0)
		{			
			glBindVertexArray(VertArray->GetVaoHandle());
			glDrawElements(GL_TRIANGLES, Cmd->IndexCount, GL_UNSIGNED_INT, reinterpret_cast<void*>(Cmd->FirstIndex * sizeof(int32)));
			glBindVertexArray(0);
		}
	}

	void GLCommandBuffer::GLBindState(BindPipelineCommand* Cmd)
	{

		GLState* State = dynamic_cast<GLState*>(Cmd->Pipeline);

		// Todo: only change this state if needed
		// This is the opengl state block - state changes should not occur outside of here

		//glEnable(GL_SCISSOR_TEST);
		const PipelineCreateInfo& StateInfo = State->GetCreateInfo();

		if (StateInfo.bEnableScissorTest)
		{
			glEnable(GL_SCISSOR_TEST);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
		}


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
			glBlendEquation(MappedBlendOps[static_cast<uint32>(StateInfo.Blend.Op)]);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_FRAMEBUFFER_SRGB);
		
		// Bind OpenGL shader
		const GLShader* Shader = dynamic_cast<const GLShader*>(Cmd->Pipeline->GetCreateInfo().PipelineShader);
		glUseProgram(Shader->GetProgramHandle());

		// Todo: Set blend parameters

		this->BoundState = dynamic_cast<GLState*>(Cmd->Pipeline);

	}

	void GLCommandBuffer::GLCmdBufferCommand(CommandBufferCommand* Cmd)
	{
		GLCommandBuffer* Secondary = dynamic_cast<GLCommandBuffer*>(Cmd->CmdBuffer);

		CORE_ASSERT(Secondary != nullptr);

		Secondary->Submit();
	}

	// Todo: how do we want to handle setting render primitives across APIs?
	// OpenGL does this per draw whereas Vulkan does this as part of the pipeline
	// Will probably need to put this as part of the pipeline with OpenGL as well
	// 
	// int32 GLCommandBuffer::PrimitiveToGL(Primitive prim) const
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

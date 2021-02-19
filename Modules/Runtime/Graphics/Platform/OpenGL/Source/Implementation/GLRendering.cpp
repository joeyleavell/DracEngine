#include "GLRendering.h"
#include "GLShader.h"
#include "GLVertexArray.h"
#include "GLTexture.h"
#include "GLFrameBuffer.h"
#include <iostream>
#include "glew.h"

namespace Ry
{

	void GLRenderer::set_clear_color(const Vector4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	
	void GLRenderer::clear_screen()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void GLRenderer::set_render_mode(const RenderMode& mode)
	{
		switch (mode)
		{
		case Ry::RenderMode::FILLED:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case Ry::RenderMode::WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		}
	}

	int32 GLRenderer::GetGraphicsError()
	{
		return glGetError();
	}

	void GLRenderer::set_viewport(int32 x, int32 y, int32 width, int32 height)
	{
		glViewport(x, y, width, height);
	}

	void GLRenderer::SetBlendingEnabled(bool BlendEnabled)
	{
		if(BlendEnabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}
	
	void GLRenderer::SetBlendMode(BlendMode Mode)
	{
		glBlendEquation(BlendModeToGLBlendEquation(Mode));
	}
	
	void GLRenderer::SetBlendFactors(BlendFactor SourceFactor, BlendFactor DestinationFactor)
	{
		glBlendFunc(BlendFactorToGLBlendFunction(SourceFactor), BlendFactorToGLBlendFunction(DestinationFactor));
	}

	void GLRenderer::SetBlendFactorsSeparate(BlendFactor SourceColorFactor, BlendFactor DestinationColorFactor, BlendFactor SourceAlphaFactor, BlendFactor DestAlphaFactor)
	{
		glBlendFuncSeparate(BlendFactorToGLBlendFunction(SourceColorFactor), BlendFactorToGLBlendFunction(DestinationColorFactor), BlendFactorToGLBlendFunction(SourceAlphaFactor), BlendFactorToGLBlendFunction(DestAlphaFactor));
	}

	bool GLRenderer::IsBlendingEnabled()
	{
		GLboolean BlendingEnabled;
		glGetBooleanv(GL_BLEND, &BlendingEnabled);

		return BlendingEnabled;
	}
	
	BlendMode GLRenderer::GetBlendMode()
	{
		GLint BlendEquation;
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &BlendEquation);

		return GLBlendEquationToBlendMode(BlendEquation);
	}
	
	BlendFactor GLRenderer::GetSourceBlendFactor()
	{
		GLint BlendFunction;
		glGetIntegerv(GL_BLEND_SRC_RGB, &BlendFunction);

		return GLBlendFunctionToBlendFactor(BlendFunction);
	}

	BlendFactor GLRenderer::GetDestBlendFactor()
	{
		GLint BlendFunction;
		glGetIntegerv(GL_BLEND_DST_RGB, &BlendFunction);

		return GLBlendFunctionToBlendFactor(BlendFunction);
	}

	void GLRenderer::SetDepthTestEnabled(bool Enabled)
	{
		if(Enabled)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
	
	void GLRenderer::ClearBuffers(bool Color, bool Depth)
	{
		int32 Buffers = 0;

		if(Color)
		{
			Buffers |= GL_COLOR_BUFFER_BIT;
		}

		if(Depth)
		{
			Buffers |= GL_DEPTH_BUFFER_BIT;
		}

		glClear(Buffers);
	}

	void GLRenderer::SetClearColor(const Ry::Vector3& ClearColor)
	{
		glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, 1.0);
	}

	void GLRenderer::BindDefaultFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLRenderer::SetMSAAEnabled(bool Enabled)
	{
		if(Enabled)
		{
			glEnable(GL_MULTISAMPLE);
		}
		else
		{
			glDisable(GL_MULTISAMPLE);
		}
	}

	void GLRenderer::SetViewport(int32 X, int32 Y, int32 Width, int32 Height)
	{
		glViewport(X, Y, Width, Height);
	}

	uint32 GLRenderer::BlendModeToGLBlendEquation(BlendMode Mode)
	{
		switch(Mode)
		{
		case BlendMode::ADDITIVE:
			return GL_FUNC_ADD;
		case BlendMode::SUBTRACTIVE:
			return GL_FUNC_SUBTRACT;
		case BlendMode::INVERSE_SUBTRACTIVE:
			return GL_FUNC_REVERSE_SUBTRACT;
		case BlendMode::MIN:
			return GL_MIN;
		case BlendMode::MAX:
			return GL_MAX;
		}

		Ry::Log->LogError("Blend mode " + Ry::to_string((uint32)Mode) + " is not supported");

		return 0;
	}
	
	uint32 GLRenderer::BlendFactorToGLBlendFunction(BlendFactor Factor)
	{
		switch(Factor)
		{
		case BlendFactor::ONE:
			return GL_ONE;
		case BlendFactor::ZERO:
			return GL_ZERO;
		case BlendFactor::ONE_MINUS_SRC_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::ONE_MINUS_DST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::SRC_ALPHA:
			return GL_SRC_ALPHA;
		case BlendFactor::DST_ALPHA:
			return GL_DST_ALPHA;
		}

		Ry::Log->LogError("Blend factor " + Ry::to_string((uint32)Factor) + " is not supported");
		return 0;
	}

	BlendMode GLRenderer::GLBlendEquationToBlendMode(uint32 GLBlendEquation)
	{
		switch (GLBlendEquation)
		{
		case GL_FUNC_ADD:
			return BlendMode::ADDITIVE;
		case GL_FUNC_SUBTRACT:
			return BlendMode::SUBTRACTIVE;
		case GL_FUNC_REVERSE_SUBTRACT:
			return BlendMode::INVERSE_SUBTRACTIVE;
		case GL_MIN:
			return BlendMode::MIN;
		case GL_MAX:
			return BlendMode::MAX;
		}

		Ry::Log->LogError("GL blend equation " + Ry::to_string((uint32)GLBlendEquation) + " does not map to any blend modes");
		return BlendMode::NONE;
	}
	
	BlendFactor GLRenderer::GLBlendFunctionToBlendFactor(uint32 GLBlendFunction)
	{
		switch (GLBlendFunction)
		{
		case GL_ONE:
			return BlendFactor::ONE;
		case GL_ZERO:
			return BlendFactor::ZERO;
		case GL_ONE_MINUS_SRC_ALPHA:
			return BlendFactor::ONE_MINUS_SRC_ALPHA;
		case GL_ONE_MINUS_DST_ALPHA:
			return BlendFactor::ONE_MINUS_DST_ALPHA;
		case GL_SRC_ALPHA:
			return BlendFactor::SRC_ALPHA;
		case GL_DST_ALPHA:
			return BlendFactor::DST_ALPHA;
		}

		Ry::Log->LogError("GL blend function " + Ry::to_string((uint32)GLBlendFunction) + " does not map to any blend factors");
		return BlendFactor::NONE;
	}

	VertexArray* GLRenderer::make_vertex_array(const VertexFormat& format, bool with_index_buffer)
	{
		return new GLVertexArray(format, with_index_buffer);
	}
	
	Shader* GLRenderer::make_shader(const VertexFormat& format, const Ry::AssetRef& VertAsset, const Ry::AssetRef& FragAsset)
	{
		return new Ry::GLShader(format, VertAsset, FragAsset);
	}
	
	Texture* GLRenderer::make_texture()
	{
		return new GLTexture();
	}

	FrameBuffer* GLRenderer::make_framebuffer(int32 Width, int32 Height, int32 Samples)
	{
		return new GLFrameBuffer(Width, Height, Samples);
	}

	// bool InitOGLRendering()
	// {
	// 	RenderAPI = new GLRenderer;
	//
	// 	if (glewInit() != GLEW_OK)
	// 	{
	// 		Ry::Log->LogError("Failed to load OpenGL symbols");
	// 		return false;
	// 	}
	// 	else
	// 	{
	// 		Ry::Log->Log("OpenGL symbols loaded");
	//
	// 		int major;
	// 		int minor;
	// 		int32 FBOSamples;
	// 		glGetIntegerv(GL_MAJOR_VERSION, &major);
	// 		glGetIntegerv(GL_MINOR_VERSION, &minor);
	// 		glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &FBOSamples);
	//
	// 		Ry::Log->Log("GL version: " + Ry::to_string(major) + "." + Ry::to_string(minor));
	// 		Ry::Log->Log("\tMax FBO samples: " + Ry::to_string(FBOSamples));
	// 	}
	//
	// 	return true;
	// }

}

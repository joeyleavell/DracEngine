#pragma once

#include "Interface/Rendering.h"
#include "Core/Globals.h"
#include "OpenGLGen.h"

namespace Ry
{

	/**
	 * OpenGL implementation of rendering interface.
	 */
	class OPENGL_MODULE GLRenderer : public Renderer
	{

	public:

		void set_clear_color(const Vector4& color) override;
		void clear_screen() override;
		int32 GetGraphicsError() override;
		void set_render_mode(const RenderMode& mode) override;
		void set_viewport(int32 x, int32 y, int32 width, int32 height) override;
		void SetBlendingEnabled(bool Transparency) override;
		void SetBlendMode(BlendMode Mode) override;
		void SetBlendFactors(BlendFactor SourceFactor, BlendFactor DestinationFactor) override;
		void SetBlendFactorsSeparate(BlendFactor SourceColorFactor, BlendFactor DestinationColorFactor, BlendFactor SourceAlphaFactor, BlendFactor DestAlphaFactor) override;
		bool IsBlendingEnabled() override;
		BlendMode GetBlendMode() override;
		BlendFactor GetSourceBlendFactor() override;
		BlendFactor GetDestBlendFactor() override;
		void SetDepthTestEnabled(bool Enabled) override;
		void ClearBuffers(bool Color, bool Depth) override;
		void SetClearColor(const Ry::Vector3& ClearColor) override;
		void BindDefaultFramebuffer() override;
		void SetMSAAEnabled(bool Enabled) override;
		void SetViewport(int32 X, int32 Y, int32 Width, int32 Height) override;


		virtual VertexArray* make_vertex_array(const VertexFormat& format, bool with_index_buffer);
		virtual Shader* make_shader(const VertexFormat& format, const Ry::AssetRef& VertAsset, const Ry::AssetRef& FragAsset);
		virtual Texture* make_texture();
		virtual FrameBuffer* make_framebuffer(int32 Width, int32 Height, int32 Samples);

	private:

		uint32 BlendModeToGLBlendEquation(BlendMode Mode);
		uint32 BlendFactorToGLBlendFunction(BlendFactor Factor);
		BlendFactor GLBlendFunctionToBlendFactor(uint32 GLBlendFunction);
		BlendMode GLBlendEquationToBlendMode(uint32 GLBlendEquation);
	};


	OPENGL_MODULE bool InitOGLRendering();
	

}
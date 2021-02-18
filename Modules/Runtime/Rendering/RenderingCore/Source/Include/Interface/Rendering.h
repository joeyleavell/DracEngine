#pragma once

#include "Core/Core.h"
#include "Interface/VertexArray.h"
#include "Interface/Shader.h"
#include "Interface/Texture.h"
#include "Interface/FrameBuffer.h"
#include "Vertex.h"
#include "Mesh.h"
#include <vector>
#include "RenderingCoreGen.h"

namespace Ry
{
	
	/**
	 * All supported rendering platforms.
	 */
	enum class RENDERINGCORE_MODULE RenderingPlatform
	{
		GL4
	};

	enum class RENDERINGCORE_MODULE RenderMode
	{
		FILLED, WIREFRAME
	};

	enum class RENDERINGCORE_MODULE BlendMode
	{
		NONE, ADDITIVE, SUBTRACTIVE, INVERSE_SUBTRACTIVE, MIN, MAX
	};

	enum class RENDERINGCORE_MODULE BlendFactor
	{
		NONE, SRC_ALPHA, DST_ALPHA, ONE_MINUS_SRC_ALPHA, ONE_MINUS_DST_ALPHA, ONE, ZERO
	};

	struct RENDERINGCORE_MODULE BlendParameters
	{
		bool BlendingEnabled = false;
		BlendMode Mode = BlendMode::ADDITIVE;
		BlendFactor SourceColorFactor = BlendFactor::ONE;
		BlendFactor DestColorFactor = BlendFactor::ZERO;
		BlendFactor SourceAlphaFactor = BlendFactor::ONE;
		BlendFactor DestAlphaFactor = BlendFactor::ONE;
	};

	/**
	 * Represents an abstract rendering interface.
	 */
	class RENDERINGCORE_MODULE Renderer
	{
	public:

		/**
		 * Sets the clear color for the screen.
		 * @param The clear color of the screen
		 */
		virtual void set_clear_color(const Vector4& color) = 0;
		
		/**
		 * Clears the color buffer of the screen.
		 */
		virtual void clear_screen() = 0;

		virtual void set_render_mode(const RenderMode& mode) = 0;

		virtual void SetDepthTestEnabled(bool Enabled) = 0;
		virtual void ClearBuffers(bool Color, bool Depth) = 0;
		virtual void SetClearColor(const Ry::Vector3& ClearColor) = 0;

		virtual void SetBlendingEnabled(bool Transparency) = 0;
		virtual void SetBlendMode(BlendMode Mode) = 0;
		virtual void SetBlendFactors(BlendFactor SourceFactor, BlendFactor DestinationFactor) = 0;
		virtual void SetBlendFactorsSeparate(BlendFactor SourceColorFactor, BlendFactor DestinationColorFactor, BlendFactor SourceAlphaFactor, BlendFactor DestAlphaFactor) = 0;

		virtual void SetMSAAEnabled(bool Enabled) = 0;

		virtual bool IsBlendingEnabled() = 0;
		virtual BlendFactor GetSourceBlendFactor() = 0;
		virtual BlendFactor GetDestBlendFactor() = 0;
		virtual BlendMode GetBlendMode() = 0;

		virtual void SetViewport(int32 X, int32 Y, int32 Width, int32 Height) = 0;

		virtual void BindDefaultFramebuffer() = 0;

		void SetBlendState(const BlendParameters& State);
		void PushCurrentBlendState();
		void PushBlendState(const BlendParameters& BlendState);
		void PopBlendState();
		
		virtual void set_viewport(int32 x, int32 y, int32 width, int32 height) = 0;

		virtual int32 GetGraphicsError() = 0;

		/**
		 * Makes a new vertex array using the correct rendering API.
		 */
		virtual VertexArray* make_vertex_array(const VertexFormat& format, bool with_index_buffer) = 0;

		/**
		 * Makes a new shader using the correct rendering API.
		 * @param vertName The name of the vertex shader to use
		 * @param fragName The name of the fragment shader to use
		 */
		virtual Shader* make_shader(const VertexFormat& format, const Ry::AssetRef& VertAsset, const Ry::AssetRef& FragAsset) = 0;
		
		/**
		 * Makes a 2D texture using the correct rendering API.
		 */
		virtual Texture* make_texture() = 0;

		virtual FrameBuffer* make_framebuffer(int32 Width, int32 Height, int32 Samples) = 0;

	private:

		Ry::ArrayList<BlendParameters> BlendStateStack;

	};

}
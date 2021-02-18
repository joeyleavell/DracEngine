#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "RyMath.h"
#include "Interface/VertexArray.h"
#include "Asset.h"

#define VERT_DIR "./shaders/vertex"
#define FRAG_DIR "./shaders/fragment"

namespace Ry
{

	class RENDERINGCORE_MODULE Shader
	{
	public:

		Shader(const VertexFormat& format, const AssetRef& VSAsset, const AssetRef& FSAsset)
		{
			
		}
		
		virtual ~Shader() = default;

		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/

		/**
		 * Deletes this shader from the rendering interface. Do not use shader after calling this.
		 */
		virtual void deleteShader() = 0;

		/**
		 * Uploads a uniform to the shader pipeline.
		 * @param location The integer index of the shader uniform.
		 * @param float* The uniform matrix data.
		 */
		virtual void uniformMat44(const Ry::String name, const float* data) = 0;
		
		/**
		 * Uploads a uniform to the shader pipeline.
		 * @param location The integer index of the shader uniform.
		 * @param Matrix4 The uniform matrix data.
		 */
		virtual void uniformMat44(const Ry::String name, const Matrix4& mat) = 0;
		
		/**
		 * Uploads a uniform to the shader pipeline.
		 * @param location The integer index of the shader uniform.
		 * @param Matrix4 The uniform float.
		 */
		virtual void uniform_float(const Ry::String name, float v) = 0;

		virtual void uniform_vec3(const Ry::String name, Vector3 vec) = 0;
		virtual void uniform_vec2(const Ry::String name, Vector2 vec) = 0;
		virtual void uniform_int32(const Ry::String name, int32 Num) = 0;

		/**
		 * Binds this shader for rendering usage. This should be called before rendering a vertex array. 
		 */
		virtual void bind() const = 0;

		/**
		 * Unbinds any currently bound shader. This should be called after binding the shader.
		 */
		virtual void unbind() = 0;

	};


}
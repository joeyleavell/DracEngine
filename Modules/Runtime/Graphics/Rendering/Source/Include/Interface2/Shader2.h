#pragma once

#include "Asset.h"
#include "RenderingGen.h"

namespace Ry
{

	enum class ShaderStage
	{
		Vertex,
		Fragment
	};


	/**
	 * Stores shader data types and how many conceptual elements they store (not m=
	 *
	 * All matrices in this enum are in ROW MAJOR order.
	 * todo: this would be a great use case for runtime reflection. pass in a reflected struct so we can dynamically make the uniform buffer
	 */
	enum class ShaderPrimitiveDataType
	{
		/* 1 element types */
		Float,

		/* 2 element types */
		Float2,
		Float1x2,
		Float2x1,

		/* 3 element types */
		Float3,
		Float3x1,
		Float1x3,

		/* 4 element types */
		Float4,
		Float2x2,
		Float4x1,
		Float1x4,

		/* 6 element types */
		Float2x3,
		Float3x2,

		/* 8 element types */
		Float4x2,
		Float2x4,

		/* 9 element types */
		Float3x3,

		/* 12 element types */
		Float4x3,
		Float3x4,

		/* 16 element types*/
		Float4x4
	};

	Ry::String RENDERING_MODULE ToString(ShaderPrimitiveDataType DT);

	class RENDERING_MODULE Shader2
	{
	public:

		Shader2(const Ry::String& VSAsset, const Ry::String& FSAsset)
		{

		}
		
		virtual ~Shader2() = default;

		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/
		
		/**
		 * Deletes this shader from the rendering interface. Do not use shader after calling this.
		 */
		virtual void DestroyShader() = 0;
		
	};


}
#pragma once

#include "Interface/Shader.h"
#include "GLVertexArray.h"
#include "Core/Core.h"
#include "Core/String.h"
#include "Data/Map.h"

#define MAX_UNIFORMS 30

namespace Ry
{

	class OPENGL_MODULE GLShader : public Shader
	{
	public:

		GLShader(const VertexFormat& format, const AssetRef& VSAsset, const AssetRef& FSAsset);

		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/

		virtual void deleteShader();
		virtual void uniformMat44(const Ry::String name, const float* data);
		virtual void uniformMat44(const Ry::String name, const Matrix4& mat);
		virtual void uniform_float(const Ry::String name, float v);
		virtual void uniform_vec2(const Ry::String name, Vector2 vec);
		virtual void uniform_vec3(const Ry::String name, Vector3 vec);
		virtual void uniform_int32(const Ry::String name, int32 Num);
		virtual void bind_vao(const VertexArray* vao) const;
		virtual void bind() const;
		virtual void unbind();

	private:

		// String vertName;
		// String fragName;
		uint32 programHandle;

		Map<Ry::String, int32> uniforms;

		int32 createShader(int32 type, const AssetRef& Reference);
		GLint get_uniform_location(Ry::String name);
		void programStatusCheck(int32 type, const String& name);
	};

}
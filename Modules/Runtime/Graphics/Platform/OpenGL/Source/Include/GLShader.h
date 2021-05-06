#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "Data/Map.h"
#include "Interface/Shader.h"
#include "glew.h"
#include "OpenGLGen.h"

#define MAX_UNIFORMS 30

namespace Ry
{

	class OPENGL_MODULE GLShader : public Shader
	{
	public:

		GLShader(const Ry::String& VertLoc, const Ry::String& FragLoc);
		virtual ~GLShader() = default;

		/************************************************************************/
		/* Interface functions                                                  */
		/************************************************************************/

		/**
		 * Deletes this shader from the rendering interface. Do not use shader after calling this.
		 */
		void DestroyShader() override;

		GLuint GetProgramHandle() const
		{
			return ProgramHandle;
		}

	private:
		
		uint32 ProgramHandle;

		bool CreateShader(GLuint& OutHandle, int32 Type, const Ry::String& Reference);
		bool ProgramStatusCheck(int32 Type, const String& Name);

		// todo: move these to gl resources
		// Map<Ry::String, int32> uniforms;
		// GLint get_uniform_location(Ry::String name);

	};

}
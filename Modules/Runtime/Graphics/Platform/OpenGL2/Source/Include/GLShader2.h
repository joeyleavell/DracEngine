#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "Data/Map.h"
#include "Interface2/Shader2.h"
#include "glew.h"
#include "OpenGL2Gen.h"

#define MAX_UNIFORMS 30

namespace Ry
{

	class OPENGL2_MODULE GLShader2 : public Shader2
	{
	public:

		GLShader2(const Ry::String& VertLoc, const Ry::String& FragLoc);
		virtual ~GLShader2() = default;

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
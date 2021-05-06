#include "GLShader.h"
#include "Core/Globals.h"
#include "TextFileAsset.h"
#include "Manager/IAssetManager.h"
#include "Vertex.h"
#include "Language/ShaderCompiler.h"

namespace Ry
{

	GLShader::GLShader(const Ry::String& VSAsset, const Ry::String& FSAsset):
	Shader(VSAsset, FSAsset)
	{
		// todo: transpile source from hlsl to glsl
		
		// Create the entire OpenGL program.
		this->ProgramHandle = glCreateProgram();

		// Create and compile the vertex and fragment shaders.
		GLuint VertShaderHandle;
		GLuint FragShaderHandle;

		if(!CreateShader(VertShaderHandle, GL_VERTEX_SHADER, VSAsset))
		{
			return;
		}
		
		if(!CreateShader(FragShaderHandle, GL_FRAGMENT_SHADER, FSAsset))
		{
			return;
		}

		// Attach the compiled shaders.
		glAttachShader(ProgramHandle, VertShaderHandle);
		glAttachShader(ProgramHandle, FragShaderHandle);

		// todo: this stuff should be explicitly defined in the shader
		// for (int32 i = 0; i < format.attribute_count; i++)
		// {
		// 	VertexAttrib attrib = format.attributes[i];
		// 	String name = attrib.Name;
		// 	glBindAttribLocation(programHandle, i, *name);
		// }

		// Link and check the status.
		glLinkProgram(ProgramHandle);
		if(!ProgramStatusCheck(GL_LINK_STATUS, "link"))
		{
			return;
		}

		// Validate and check the status.
		glValidateProgram(ProgramHandle);
		if(!ProgramStatusCheck(GL_VALIDATE_STATUS, "validate"))
		{
			return;
		}

		// Detach and delete unneeded resource.
		glDetachShader(ProgramHandle, VertShaderHandle);
		glDetachShader(ProgramHandle, FragShaderHandle);
		glDeleteShader(VertShaderHandle);
		glDeleteShader(FragShaderHandle);
	}

	bool GLShader::CreateShader(GLuint& OutHandle, int32 Type, const Ry::String& ShaderLoc)
	{
		// Load the text file asset
		//String HLSLShaderSource = AssetMan->LoadAsset(Reference, ASSET_TYPE_TEXT)->As<TextFileAsset>()->GetContents();
		//AssetMan->UnloadAsset(Reference);

		// Transpile the HLSL to GLSL
		Ry::String OutGLSL;
		Ry::String ErrWarn;
		if(!CompileToGlsl(ShaderLoc, OutGLSL, ErrWarn, Type == GL_VERTEX_SHADER ? ShaderStage::Vertex : ShaderStage::Fragment))
		{
			Ry::Log->LogErrorf("Failed to compile shader %s:\n%s", *ShaderLoc, *ErrWarn);
			return false;
		}
		else
		{
			std::cout << "Out glsl: \n" << *OutGLSL << std::endl << std::endl;
		}

		int32 Handle = glCreateShader(Type);

		const char* Data = OutGLSL.getData();
		const char* const* PtrToData = &Data;
		int32 Size = OutGLSL.getSize();
		glShaderSource(Handle, 1, PtrToData, &Size);

		glCompileShader(Handle);

		int32 Result;
		glGetShaderiv(Handle, GL_COMPILE_STATUS, &Result);

		if (Result == GL_FALSE)
		{
			GLsizei Length;
			GLchar InfoLog[1024];
			glGetShaderInfoLog(Handle, 1024, &Length, InfoLog);

			Ry::Log->LogErrorf("Shader %s failed to compile: %s", *ShaderLoc, InfoLog);
			return false;
		}
		else
		{
			Ry::Log->Logf("%s compiled", *ShaderLoc);
		}

		OutHandle = Handle;

		return true;
	}

	bool GLShader::ProgramStatusCheck(int32 Type, const String& Name)
	{
		int32 Result;
		glGetProgramiv(ProgramHandle, Type, &Result);

		if (Result == GL_FALSE)
		{
			char Log[2048];
			int Length;
			glGetProgramInfoLog(ProgramHandle, 2048, &Length, Log);

			Ry::Log->LogErrorf("GLSL program %s error:", *Name);
			Ry::Log->LogError(Log);

			return false;
		}
		else
		{
			return true;
		}
	}

	void GLShader::DestroyShader()
	{
		glDeleteProgram(ProgramHandle);
	}

	// todo: move these to gl resources
	// void GLShader::uniformMat44(const Ry::String name, const float* data)
	// {
	// 	glProgramUniformMatrix4fv(programHandle, get_uniform_location(name), 1, true, data);
	// }
	//
	// void GLShader::uniformMat44(const Ry::String name, const Matrix4& mat)
	// {
	// 	glProgramUniformMatrix4fv(programHandle, get_uniform_location(name), 1, true, *mat);
	// }
	//
	// void GLShader::uniform_float(const Ry::String name, float v)
	// {
	// 	glProgramUniform1f(programHandle, get_uniform_location(name), v);
	// }
	//
	// void GLShader::uniform_vec2(const Ry::String name, Vector2 vec)
	// {
	// 	glProgramUniform2f(programHandle, get_uniform_location(name), vec.x, vec.y);
	// }
	//
	// void GLShader::uniform_vec3(const Ry::String name, Vector3 vec)
	// {
	// 	glProgramUniform3f(programHandle, get_uniform_location(name), vec.x, vec.y, vec.z);
	// }
	//
	// void GLShader::uniform_int32(const Ry::String name, int32 Num)
	// {
	// 	glProgramUniform1i(programHandle, get_uniform_location(name), Num);
	// }
	//
	// void GLShader::bind() const
	// {
	// 	glUseProgram(programHandle);
	// }
	//
	// void GLShader::bind_vao(const VertexArray* vao) const
	// {
	// }
	//
	// void GLShader::unbind()
	// {
	// 	glUseProgram(0);
	// }
	//
	// GLint GLShader::get_uniform_location(Ry::String name)
	// {
	// 	if (uniforms.contains(name))
	// 		return *uniforms.get(name);
	// 	else
	// 	{
	// 		int32 location = glGetUniformLocation(programHandle, *name);
	// 		uniforms.insert(name, location);
	// 		return location;
	// 	}
	// }

}
#pragma once

#include "GLShader.h"
#include "Core/Globals.h"
#include "TextFileAsset.h"
#include "Manager/IAssetManager.h"
#include <iostream>

namespace Ry
{

	GLShader::GLShader(const VertexFormat& format, const AssetRef& VSAsset, const AssetRef& FSAsset):
		Shader(format, VSAsset, FSAsset)
	{
		int major;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		
		// Create the entire OpenGL program.
		this->programHandle = glCreateProgram();

		// Create and compile the vertex and fragment shaders.
		int32 vertHandle = createShader(GL_VERTEX_SHADER, VSAsset);
		int32 fragHandle = createShader(GL_FRAGMENT_SHADER, FSAsset);

		// Attach the compiled shaders.
		glAttachShader(programHandle, vertHandle);
		glAttachShader(programHandle, fragHandle);

		for (int32 i = 0; i < format.attribute_count; i++)
		{
			VertexAttrib attrib = format.attributes[i];
			String name = attrib.name;
			glBindAttribLocation(programHandle, i, *name);
		}

		// Link and check the status.
		glLinkProgram(programHandle);
		programStatusCheck(GL_LINK_STATUS, "link");

		// Validate and check the status.
		glValidateProgram(programHandle);
		programStatusCheck(GL_VALIDATE_STATUS, "validate");

		// Detach and delete unneeded resource.
		glDetachShader(programHandle, vertHandle);
		glDetachShader(programHandle, fragHandle);
		glDeleteShader(vertHandle);
		glDeleteShader(fragHandle);
	}

	void GLShader::deleteShader()
	{
		glDeleteProgram(programHandle);
	}

	void GLShader::uniformMat44(const Ry::String name, const float* data)
	{
		glProgramUniformMatrix4fv(programHandle, get_uniform_location(name), 1, true, data);
	}

	void GLShader::uniformMat44(const Ry::String name, const Matrix4& mat)
	{
		glProgramUniformMatrix4fv(programHandle, get_uniform_location(name), 1, true, *mat);
	}

	void GLShader::uniform_float(const Ry::String name, float v)
	{
		glProgramUniform1f(programHandle, get_uniform_location(name), v);
	}

	void GLShader::uniform_vec2(const Ry::String name, Vector2 vec)
	{
		glProgramUniform2f(programHandle, get_uniform_location(name), vec.x, vec.y);
	}

	void GLShader::uniform_vec3(const Ry::String name, Vector3 vec)
	{
		glProgramUniform3f(programHandle, get_uniform_location(name), vec.x, vec.y, vec.z);
	}

	void GLShader::uniform_int32(const Ry::String name, int32 Num)
	{
		glProgramUniform1i(programHandle, get_uniform_location(name), Num);
	}

	void GLShader::bind() const
	{
		glUseProgram(programHandle);
	}

	void GLShader::bind_vao(const VertexArray* vao) const
	{
	}

	void GLShader::unbind()
	{
		glUseProgram(0);
	}

	int32 GLShader::createShader(int32 type, const Ry::AssetRef& Reference)
	{
		// Load the text file asset
		String ShaderSource = AssetMan->LoadAsset(Reference, ASSET_TYPE_TEXT)->As<TextFileAsset>()->GetContents();
		AssetMan->UnloadAsset(Reference);

		int32 handle = glCreateShader(type);

		const char* data = ShaderSource.getData();
		const char* const* ptr_to_data = &data;
		int32 size = ShaderSource.getSize();
		glShaderSource(handle, 1, ptr_to_data, &size);

		glCompileShader(handle);

		int32 result;
		glGetShaderiv(handle, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE)
		{
			GLsizei length;
			GLchar info_log[1024];
			glGetShaderInfoLog(handle, 1024, &length, info_log);

			Ry::Log->LogError("Shader failed to compile: " + Reference.GetVirtual());
		}
		else
		{
			Ry::Log->Log(Reference.GetVirtual() + " compiled");
		}

		return handle;
	}

	GLint GLShader::get_uniform_location(Ry::String name)
	{
		if (uniforms.contains(name))
			return *uniforms.get(name);
		else
		{
			int32 location = glGetUniformLocation(programHandle, *name);
			uniforms.insert(name, location);
			return location;
		}
	}

	void GLShader::programStatusCheck(int32 type, const String& name)
	{
		int32 result;
		glGetProgramiv(programHandle, type, &result);

		if (result == GL_FALSE)
		{
			char log[2048];
			int length;
			glGetProgramInfoLog(programHandle, 2048, &length, log);

			Ry::Log->LogError("GLSL program " + name + " error: ");
			Ry::Log->LogError(log);
		}
	}
}
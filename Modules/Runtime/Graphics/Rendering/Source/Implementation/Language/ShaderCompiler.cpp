#include "Language/ShaderCompiler.h"
#include "File/File.h"
#include "Core/Globals.h"
#include "Language/ShaderReflector.h"

#ifndef RYBUILD_DISTRIBUTE
	#include "ShaderConductor.hpp"
#endif

namespace Ry
{

	/**
 * Retrieve the HLSL variant of the shader location.
 */
	Ry::String GetHlslLocAbs(const Ry::String& ShaderLoc)
	{
		Ry::String HLSLRoot = Ry::File::Join("Shaders", "Hlsl");
		Ry::String ShaderLocVirtual = Ry::File::Join(HLSLRoot, ShaderLoc + ".hlsl");

		ShaderLocVirtual.Replace('\\', '/');

		return Ry::File::VirtualToAbsolute(ShaderLocVirtual);
	}

	Ry::String GetSpirVLocAbs(const Ry::String& ShaderLoc)
	{
		Ry::String HLSLRoot = Ry::File::Join("Shaders", "SpirV");
		Ry::String ShaderLocVirtual = Ry::File::Join(HLSLRoot, ShaderLoc + ".spirv");

		ShaderLocVirtual.Replace('\\', '/');

		return Ry::File::VirtualToAbsolute(ShaderLocVirtual);
	}

	Ry::String GetGlslLocAbs(const Ry::String& ShaderLoc)
	{
		Ry::String HLSLRoot = Ry::File::Join("Shaders", "Glsl");
		Ry::String ShaderLocVirtual = Ry::File::Join(HLSLRoot, ShaderLoc + ".glsl");

		ShaderLocVirtual.Replace('\\', '/');

		return Ry::File::VirtualToAbsolute(ShaderLocVirtual);
	}


	bool GetHlslSource(const Ry::String& ShaderLoc, Ry::String& OutSource)
	{
		Ry::String ShaderLocAbs = GetHlslLocAbs(ShaderLoc);

		if (!Ry::File::DoesFileExist(ShaderLocAbs))
		{
			Ry::Log->LogErrorf("HLSL shader location does not exist: %s", *ShaderLocAbs);
			return false;
		}
		else
		{
			OutSource = Ry::File::LoadFileAsString2(ShaderLocAbs);
			return true;
		}
	}


	// Only compile shaders in non distributed builds
#ifndef RYBUILD_DISTRIBUTE
	ShaderConductor::Compiler::SourceDesc MakeSrcDsc(const Ry::String& Source, ShaderStage Stage)
	{
		ShaderConductor::Compiler::SourceDesc Src;
		Src.entryPoint = "main";
		Src.defines = nullptr;
		Src.fileName = nullptr;
		Src.numDefines = 0;
		Src.source = *Source;

		if (Stage == ShaderStage::Vertex)
		{
			Src.stage = ShaderConductor::ShaderStage::VertexShader;
		}
		else if (Stage == ShaderStage::Fragment)
		{
			Src.stage = ShaderConductor::ShaderStage::PixelShader;
		}

		return Src;
	}

	void CacheSpirV(Ry::String Output, uint8* SpirV, int32 Size)
	{
		
	}

	bool HLSLtoSPIRV(const Ry::String& Source, uint8*& OutSpirV, int32& OutSize, Ry::String& ErrWarnMsg, ShaderStage Stage)
	{
		if (Source.IsEmpty())
		{
			ErrWarnMsg = "Source string was empty";
			return false;
		}

		ShaderConductor::Compiler::SourceDesc Src = MakeSrcDsc(Source, Stage);

		ShaderConductor::Compiler::Options Opt; // HLSL shader options
		Opt.disableOptimizations = true;
		Opt.shiftAllCBuffersBindings = C_BUFFER_OFFSET;
		Opt.shiftAllTexturesBindings = T_BUFFER_OFFSET;
		Opt.shiftAllSamplersBindings = S_BUFFER_OFFSET;
		Opt.shiftAllUABuffersBindings = U_BUFFER_OFFSET;

		ShaderConductor::Compiler::TargetDesc TargetDsc;
		TargetDsc.language = ShaderConductor::ShadingLanguage::SpirV;
		TargetDsc.version = "1.0";
		TargetDsc.asModule = false;

		ShaderConductor::Compiler Comp;
		ShaderConductor::Compiler::ResultDesc Result = Comp.Compile(Src, Opt, TargetDsc);

		// Set the error/warning message output
		if (Result.errorWarningMsg.Data())
		{
			ErrWarnMsg = static_cast<const char*>(Result.errorWarningMsg.Data());
		}

		if (Result.hasError)
		{
			return false;
		}

		// Copy the data out into the output buffer
		ShaderConductor::Blob Res = Result.target;
		OutSpirV = new uint8[Res.Size()];
		OutSize = Res.Size();
		for (int32 Byte = 0; Byte < Res.Size(); Byte++)
		{
			OutSpirV[Byte] = (static_cast<const uint8*>(Res.Data()))[Byte];
		}

		return true;
	}

	bool HLSLtoGLSL(const Ry::String& Source, Ry::String& OutSource, Ry::String& ErrWarnMsg, ShaderStage Stage)
	{
		ShaderConductor::Compiler::SourceDesc Src = MakeSrcDsc(Source, Stage);
		ShaderConductor::Compiler::Options Opt; // HLSL shader options
		Opt.disableOptimizations = true;

		ShaderConductor::Compiler::TargetDesc TargetDsc;
		TargetDsc.language = ShaderConductor::ShadingLanguage::Glsl;
		TargetDsc.version = "460";
		TargetDsc.asModule = false;

		ShaderConductor::Compiler Comp;
		ShaderConductor::Compiler::ResultDesc Result = Comp.Compile(Src, Opt, TargetDsc);

		// Set the error/warning message output
		if (Result.errorWarningMsg.Data())
		{
			ErrWarnMsg = static_cast<const char*>(Result.errorWarningMsg.Data());
		}

		if (Result.hasError)
		{
			return false;
		}

		// Set the output GLSL
		ShaderConductor::Blob Res = Result.target;
		OutSource = Ry::String(static_cast<const char*>(Res.Data()), Res.Size());
		OutSource += "\0";

		return true;
	}


	bool CompileAll(Ry::String OutputDir)
	{
		// Todo: Use Dst

		Ry::String ParentLoc = "Shaders/Hlsl";
		Ry::String Abs = Ry::File::VirtualToAbsolute(ParentLoc);

		Filesystem::recursive_directory_iterator ShaderItr(*Abs);

		for (auto& ShaderPath : ShaderItr)
		{
			// Only compile hlsl files
			if (ShaderPath.path().extension() == ".hlsl")
			{
				// Convert absolute to virtual by chopping off part before shaders

				Ry::String VirtualOriginal = Ry::File::AbsoluteToVirtual(Filesystem::absolute(ShaderPath).string().c_str());
				Ry::String ShaderLoc = VirtualOriginal;

				// Remove the . from the virtual path
				int32 FirstDot = ShaderLoc.find_first(".", 0);
				ShaderLoc = ShaderLoc.substring(0, FirstDot);


				// Remove "HLSL" from beginning of path string
				if (ShaderLoc.find_first("Shaders/Hlsl", 0) == 0)
				{
					ShaderLoc = ShaderLoc.substring(Ry::String("Shaders/Hlsl").getSize());
				}

				if (!ShaderLoc.IsEmpty())
				{
					// std::cout << "Abs: " << *Abs << std::endl;
					// std::cout << "Virtual: " << *Virtual << std::endl;

					// Determine shader stage based on if the filepath contains vertex/fragment (for now)
					// Todo: make this more robust
					ShaderStage Stage;
					if (ShaderLoc.find_first("Vertex", 0) >= 0)
					{
						Stage = ShaderStage::Vertex;
					}
					else if (ShaderLoc.find_first("Fragment", 0) >= 0)
					{
						Stage = ShaderStage::Fragment;
					}

					Ry::String ErrWarn;

					Ry::String HlslSource;
					if (!GetHlslSource(ShaderLoc, HlslSource))
					{
						return false;
					}

					// Compile the spirv shader
					{
						Ry::String SpirVLocAbs = Ry::File::Join(OutputDir, "Shaders");
						SpirVLocAbs = Ry::File::Join(SpirVLocAbs, "SpirV");
						SpirVLocAbs = Ry::File::Join(SpirVLocAbs, ShaderLoc + ".spirv");

						uint8* OutSpirv;
						int32 OutSize;
						if (!HLSLtoSPIRV(HlslSource, OutSpirv, OutSize, ErrWarn, Stage))
						{
							Ry::Log->LogErrorf("Error compiling SpirV shader %s: \n%s", *SpirVLocAbs, *ErrWarn);
							return false;
						}

						// Write out file to proper location

						// Create directories
						Ry::File::MakeDirectories(Ry::File::GetParentPath(SpirVLocAbs));

						// Cache the results
						std::ofstream SpirVOut;
						SpirVOut.open(*SpirVLocAbs, std::ios::binary | std::ios::out);
						{
							SpirVOut.write(reinterpret_cast<const char*>(OutSpirv), OutSize);
						}
						SpirVOut.close();
					}

					// Compile the glsl shader
					{
						Ry::String GlslLocAbs = Ry::File::Join(OutputDir, "Shaders");
						GlslLocAbs = Ry::File::Join(GlslLocAbs, "Glsl");
						GlslLocAbs = Ry::File::Join(GlslLocAbs, ShaderLoc + ".glsl");

						Ry::File::MakeDirectories(Ry::File::GetParentPath(GlslLocAbs));

						Ry::String GLSL;
						if (!HLSLtoGLSL(HlslSource, GLSL, ErrWarn, Stage))
						{
							Ry::Log->LogErrorf("Error compiling glsl shader %s: \n%s", *GLSL, *ErrWarn);
							return false;
						}

						std::ofstream GlslOut;
						GlslOut.open(*GlslLocAbs, std::ios::binary | std::ios::out);
						{
							GlslOut << *GLSL << std::endl;
						}
						GlslOut.close();
					}
				}

			}
		}

		return true;
	}
#endif
	
	bool CompileToSpirV(const Ry::String& ShaderLoc, uint8*& OutSpirV, int32& OutSize, Ry::String& ErrWarnMsg, ShaderStage Stage)
	{
		Ry::String SpirVLocAbs = GetSpirVLocAbs(ShaderLoc);

#ifdef RYBUILD_DISTRIBUTE
		// Cannot have a cache miss in a distributed build, error
		if (!Ry::File::DoesFileExist(*SpirVLocAbs))
		{
			Ry::Log->LogErrorf("Cache miss for SpirV shader: %s", *ShaderLoc);
			return false;
		}
		else
		{
			uint32 Size = 1024 * 10;
			OutSpirV = new uint8[Size];
			OutSize = Ry::File::LoadFileBytes(SpirVLocAbs, OutSpirV, Size);			
			return true;
		}
#else
		Ry::String HlslLocAbs = GetHlslLocAbs(ShaderLoc);
		bool bCacheMiss = true;

		if(Ry::File::DoesFileExist(SpirVLocAbs))
		{
			if (Ry::File::LastFileWrite(HlslLocAbs) <= Ry::File::LastFileWrite(SpirVLocAbs))
			{
				bCacheMiss = false;
			}
		}

		if(bCacheMiss)
		{
			// Cache miss, can only deal with successfully if not in distribute mode

			// Load the contents of the original HLSL
			Ry::String HlslSource;

			if (!GetHlslSource(ShaderLoc, HlslSource))
			{
				return false;
			}

			// This sets the output parameter, allows us to use it later
			if (!HLSLtoSPIRV(HlslSource, OutSpirV, OutSize, ErrWarnMsg, Stage))
			{
				// Set output parameters
				return false;
			}

			// Create directories
			Ry::File::MakeDirectories(Ry::File::GetParentPath(SpirVLocAbs));

			// Cache the results
			std::ofstream SpirVOut;
			SpirVOut.open(*SpirVLocAbs, std::ios::binary | std::ios::out);
			{
				SpirVOut.write(reinterpret_cast<const char*>(OutSpirV), OutSize);
			}
			SpirVOut.close();

			return true;
		}
		else
		{
			// Cache hit, load file
			// Todo: make this not have a max size
			uint32 Size = 1024 * 100;
			OutSpirV = new uint8[Size];
			OutSize = Ry::File::LoadFileBytes(SpirVLocAbs, OutSpirV, Size);

			return true;
		}
#endif

	}
	
	bool CompileToGlsl(const Ry::String& ShaderLoc, Ry::String& OutSource, Ry::String& ErrWarnMsg, ShaderStage Stage)
	{
		
		Ry::String GlslLocAbs = GetGlslLocAbs(ShaderLoc);
#ifdef RYBUILD_DISTRIBUTE
		// Cannot have a cache miss in a distributed build, error
		if (!Ry::File::DoesFileExist(*GlslLocAbs))
		{
			Ry::Log->LogErrorf("Cache miss for Glsl shader: %s", *ShaderLoc);
			return false;
		}
		else
		{
			OutSource = Ry::File::LoadFileAsString2(GlslLocAbs);
			return true;
		}
#else		
		Ry::String HlslLocAbs = GetHlslLocAbs(ShaderLoc);
		bool bCacheMiss = true;

		// Check if a) the cache file exists and b) it's at least as new as the hlsl equivalent
		if (Ry::File::DoesFileExist(GlslLocAbs))
		{
			if(Ry::File::LastFileWrite(HlslLocAbs) <= Ry::File::LastFileWrite(GlslLocAbs))
			{
				bCacheMiss = false;
			}
		}

		if(bCacheMiss)
		{
			// Load the contents of the original HLSL
			Ry::String HlslSource;

			if (!GetHlslSource(ShaderLoc, HlslSource))
			{
				return false;
			}

			// This sets the output parameter, allows us to use it later
			if (!HLSLtoGLSL(HlslSource, OutSource, ErrWarnMsg, Stage))
			{
				// Set output parameters
				return false;
			}

			// Create directories
			Ry::File::MakeDirectories(Ry::File::GetParentPath(GlslLocAbs));

			// Cache the results
			std::ofstream GlslOut;
			GlslOut.open(*GlslLocAbs, std::ios::binary | std::ios::out);
			{
				GlslOut << *OutSource << std::endl;
			}
			GlslOut.close();

			return true;
		}
		else
		{
			// Cache hit, load the existing source
			OutSource = Ry::File::LoadFileAsString2(GlslLocAbs);
			return true;
		}
#endif

	}

}

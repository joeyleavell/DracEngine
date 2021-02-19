#include "Language/ShaderCompiler.h"
#include "ShaderConductor/ShaderConductor.hpp"

// Defined offsets for vulkan spirv
#define C_BUFFER_OFFSET 0
#define T_BUFFER_OFFSET 10
#define S_BUFFER_OFFSET 20
#define U_BUFFER_OFFSET 30

namespace Ry
{
	ShaderConductor::Compiler::SourceDesc MakeSrcDsc(const Ry::String& Source, ShaderStage Stage)
	{
		ShaderConductor::Compiler::SourceDesc Src;
		Src.entryPoint = "main";
		Src.defines = nullptr;
		Src.fileName = nullptr;
		Src.numDefines = 0;
		Src.source = *Source;

		if(Stage == ShaderStage::Vertex)
		{
			Src.stage = ShaderConductor::ShaderStage::VertexShader;
		}
		else if(Stage == ShaderStage::Fragment)
		{
			Src.stage = ShaderConductor::ShaderStage::PixelShader;
		}

		return Src;
	}
	
	
	bool HLSLtoSPIRV(const Ry::String& Source, uint8*& OutSpirV, int32& OutSize, Ry::String& ErrWarnMsg, ShaderStage Stage)
	{
		if(Source.IsEmpty())
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
		if(Result.errorWarningMsg.Data())
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
		for(int32 Byte = 0; Byte < Res.Size(); Byte++)
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

}
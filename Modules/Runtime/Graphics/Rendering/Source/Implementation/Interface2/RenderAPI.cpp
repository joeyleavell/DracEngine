#include "Interface2/RenderAPI.h"
#include "Interface2/Pipeline.h"
#include "Interface2/Shader2.h"
#include "Core/Globals.h"

namespace Ry
{
	Pipeline2* RenderAPI2::CreatePipelineFromShader(const PipelineCreateInfo& CreateInfo, Shader2* Src)
	{
		PipelineCreateInfo New = CreateInfo;

		const ShaderReflection& VertRef = Src->GetVertexReflectionData();
		const ShaderReflection& FragRef = Src->GetFragmentReflectionData();

		for(ResourceSetDescription* Desc : VertRef.GetResources())
		{
			New.ResourceDescriptions.Add(Desc);
		}

		for (ResourceSetDescription* Desc : FragRef.GetResources())
		{
			New.ResourceDescriptions.Add(Desc);
		}

		// Determine vertex format
		const Ry::ArrayList<ShaderInputVariable>& InputVars = VertRef.GetInputVars();

		Ry::ArrayList<VertexAttrib> Attribs;

		if(InputVars.GetSize() > 0)
		{
			// VertexFormat Format;
			// Format.attribute_count = InputVars.GetSize();
			//
			// for(int32 Index = 0; Index < InputVars.GetSize(); Index++)
			// {
			// 	const ShaderInputVariable& InputVar = InputVars[Index];
			//
			// 	Format.
			// }

		}
		else
		{
			Ry::Log->LogError("Input variables for vertex shader had size zero, can't create pipeline");
			return nullptr;
		}
		
		return CreatePipeline(New);
	}
}

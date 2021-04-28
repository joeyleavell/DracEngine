#include "Interface2/RenderAPI.h"
#include "Interface2/Pipeline.h"
#include "Interface2/Shader2.h"

namespace Ry
{
	Pipeline2* RenderAPI2::CreatePipelineFromShader(const PipelineCreateInfo& CreateInfo, Shader2* Src)
	{
		PipelineCreateInfo New = CreateInfo;

		for(ResourceSetDescription* Desc : Src->GetVertexReflectionData())
		{
			New.ResourceDescriptions.Add(Desc);
		}

		for (ResourceSetDescription* Desc : Src->GetFragmentReflectionData())
		{
			New.ResourceDescriptions.Add(Desc);
		}

		return CreatePipeline(New);
	}
}

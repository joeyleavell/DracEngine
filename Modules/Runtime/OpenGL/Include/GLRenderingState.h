#pragma once

#include "Pipeline.h"
#include "OpenGLGen.h"

namespace Ry
{

	class OPENGL_MODULE GLState : public Pipeline
	{
	public:

		GLState(const Ry::PipelineCreateInfo& CreateInfo):
		Pipeline(CreateInfo){}

		bool CreatePipeline();
		void DeletePipeline();

	private:
		
	};

}
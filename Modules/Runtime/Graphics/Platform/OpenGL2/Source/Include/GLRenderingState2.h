#pragma once

#include "Interface2/Pipeline.h"
#include "OpenGL2Gen.h"

namespace Ry
{

	class OPENGL2_MODULE GLState : public Pipeline2
	{
	public:

		GLState(const Ry::PipelineCreateInfo& CreateInfo):
		Pipeline2(CreateInfo){}

		bool CreatePipeline();
		void DeletePipeline();

	private:
		
	};

}
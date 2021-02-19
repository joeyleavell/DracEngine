#include "RenderPipeline.h" 
#include "Mesh.h"
#include "RenderingEngine.h"
#include "RenderingPass.h"

namespace Ry
{

	void ForwardRenderer::Begin()
	{
		Ry::ScenePass->BeginPass();
	}

	void ForwardRenderer::RenderMesh(SharedPtr<Mesh> Mesh, const Matrix4& Model, const Matrix4& ViewProj, const SceneLighting& Lighting)
	{
		// todo: mesh doesn't render directly anymore
		//Mesh->Render(Ry::Primitive::TRIANGLE, Model, ViewProj, &Lighting);
	}

	void ForwardRenderer::End()
	{
		Ry::ScenePass->EndPass();
		Ry::SceneImposePass->Impose();
	}
	
}

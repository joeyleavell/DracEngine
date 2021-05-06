#pragma once

#include "Core/Core.h"
#include "RyMath.h"
#include "Light.h"
#include "RenderingGen.h"
#include "Mesh2.h"

namespace Ry
{
	class Mesh;

	class RENDERING_MODULE RenderPipeline
	{

	public:

		RenderPipeline() = default;
		virtual ~RenderPipeline() = default;

		virtual void Begin() = 0;
		virtual void RenderMesh(SharedPtr<Mesh2> Mesh, const Matrix4& Model, const Matrix4& ViewProj, const SceneLighting& Lighting) = 0;
		virtual void End() = 0;

	};

	class RENDERING_MODULE ForwardRenderer : public RenderPipeline
	{
	public:

		ForwardRenderer() = default;
		virtual ~ForwardRenderer() = default;

		virtual void Begin();
		virtual void RenderMesh(SharedPtr<Mesh2> Mesh, const Matrix4& Model, const Matrix4& ViewProj, const SceneLighting& Lighting);
		virtual void End();
	};
	
}

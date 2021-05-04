#pragma once

#include "SceneRenderer.gen.h"
#include "Core/Core.h"
#include "Data/Map.h"
#include "Transform.h"

namespace Ry
{
	class RenderingCommandBuffer2;

	class Pipeline2;
	class ResourceSet;
	class VertexArray2;
	class ScenePrimitive;
	class SceneGraph;
	class ResourceSetDescription;

	// Stores resources for a particular material in a primitive
	struct SCENEGRAPH_MODULE MaterialResources
	{
		// The array of resources for this material, usually this includes the below
		// MaterialSpecificResource as well as re-used ones from the scene and primitive
		Ry::ArrayList<Ry::ResourceSet*> Resources;

		// The resource set specific to this material
		Ry::ResourceSet* MaterialSpecificResource;

		int32 StartIndex;
		int32 IndexCount;

		MaterialResources();
		
	};

	// Stores resources associated with a particular primitive
	class SCENEGRAPH_MODULE PrimitiveResources
	{
	public:

		PrimitiveResources();
		virtual ~PrimitiveResources();

		const Ry::ArrayList<MaterialResources*>& GetMaterialResources() const;

		// Takes ownership of the passed in resource, will destroy when this object is freed
		void AddMaterialResource(MaterialResources* Res);

		void SetPrimitiveSpecificResources(ResourceSet* PrimSpec);
		ResourceSet* GetPrimitiveSpecificResources();

		void SetVertexArray(VertexArray2* Verts);
		VertexArray2* GetVertexArray();

		void RecordDraw(Ry::RenderingCommandBuffer2* DstBuffer);

	private:

		Ry::VertexArray2* VertArray;
		Ry::ResourceSet* PrimSpecificResource;
		
		Ry::ArrayList<MaterialResources*> MaterialRes;

	};

	class SCENEGRAPH_MODULE SceneRenderer
	{
	public:

		// ---------- Scene properties

		// Combined view and projection matrix, usually the inverse of the camera matrix
		Ry::Matrix4 ViewProjectionMatrix;
		Ry::Vector3 ViewPos;

		SceneRenderer(Ry::SceneGraph* TarScene);
		virtual ~SceneRenderer();

		virtual void Init();
		virtual void Render() = 0;
		virtual RenderingCommandBuffer2* GetCmdBuffer() = 0;

	protected:

		virtual void CreateResources() = 0;
		virtual PrimitiveResources* CreatePrimitiveResources(ScenePrimitive* Prim);
		
		// TODO: need some way of deciding which pipeline to use based on primitive
		// i.e.: flat shading, pbr shading, etc.
		Pipeline2* SelectPipelineForPrimitive();

		bool bDirty;

		Ry::SceneGraph* TargetScene;

		// Resources
		Ry::ResourceSet* SceneResources; // Per scene resources (camera, lights, etc.)
		Ry::ResourceSet* LightResources; // todo: combine with above?

		// Resource descriptions
		const Ry::ResourceSetDescription* SceneResDesc; // Per scene resource description
		const Ry::ResourceSetDescription* LightResDesc; // Per primitive resource description
		const Ry::ResourceSetDescription* PrimResDesc; // Per primitive resource description
		const Ry::ResourceSetDescription* MaterialDesc; // Per material resource description

		// A map of all scene primitives to their associated resources
		Ry::Map<ScenePrimitive*, PrimitiveResources*> PrimResources;

	private:

		void OnPrimitiveAdded(ScenePrimitive* Prim);
		void OnPrimitiveRemoved(ScenePrimitive* Prim);

	};



}

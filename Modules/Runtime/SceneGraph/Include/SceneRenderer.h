#pragma once

#include "Core/Core.h"
#include "Data/Map.h"
#include "Transform.h"
#include "SceneRenderer.gen.h"

namespace Ry
{
	class CommandBuffer;

	class Pipeline;
	class ResourceSet;
	class VertexArray;
	class ScenePrimitive;
	class SceneGraph;
	class ResourceLayout;

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

		void SetVertexArray(VertexArray* Verts);
		VertexArray* GetVertexArray();

		void RecordDraw(Ry::CommandBuffer* DstBuffer);

	private:

		Ry::VertexArray* VertArray;
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
		virtual CommandBuffer* GetCmdBuffer() = 0;

	protected:

		virtual void CreateResources() = 0;
		virtual PrimitiveResources* CreatePrimitiveResources(ScenePrimitive* Prim);
		
		// TODO: need some way of deciding which pipeline to use based on primitive
		// i.e.: flat shading, pbr shading, etc.
		Pipeline* SelectPipelineForPrimitive();

		bool bDirty;

		Ry::SceneGraph* TargetScene;

		// Resources
		Ry::ResourceSet* SceneResources; // Per scene resources (camera, lights, etc.)
		Ry::ResourceSet* LightResources; // todo: combine with above?

		// Resource descriptions
		const Ry::ResourceLayout* SceneResDesc; // Per scene resource description
		const Ry::ResourceLayout* LightResDesc; // Per primitive resource description
		const Ry::ResourceLayout* PrimResDesc; // Per primitive resource description
		const Ry::ResourceLayout* MaterialDesc; // Per material resource description

		// A map of all scene primitives to their associated resources
		Ry::Map<ScenePrimitive*, PrimitiveResources*> PrimResources;

	private:

		void OnPrimitiveAdded(ScenePrimitive* Prim);
		void OnPrimitiveRemoved(ScenePrimitive* Prim);

	};



}

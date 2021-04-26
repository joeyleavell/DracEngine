#pragma once

#include "Core/Core.h"
#include "SceneGraphGen.h"

namespace Ry
{

	class ScenePrimitive;
	
	class SCENEGRAPH_MODULE SceneGraph
	{
	public:

		// Todo: this is where some high level rendering features get implemented:
		// Frustum culling
		// Transparency sorting
		// Object grouping
		// Spatial representations (oct-tree, etc.)

		MulticastDelegate<ScenePrimitive*> OnPrimitiveAdded;
		MulticastDelegate<ScenePrimitive*> OnPrimitiveRemoved;

		SceneGraph();

		void AddPrimitive(ScenePrimitive* Prim);
		void RemovePrimitive(ScenePrimitive* Prim);

		void BeginLoading();
		void EndLoading();
		bool IsLoading();

		/**
		 * This is intended to be called before rendering the scene.
		 *
		 * Very important function.
		 * 
		 */
		void Update(float DeltaTime);

	private:

		bool bIsLoading;
		Ry::ArrayList<ScenePrimitive*> Primitives;
	};

}

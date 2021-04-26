#include "SceneGraph.h"
#include "ScenePrimitive.h"

namespace Ry
{
	SceneGraph::SceneGraph()
	{
		this->bIsLoading = false;
	}

	void SceneGraph::AddPrimitive(ScenePrimitive* Prim)
	{
		Primitives.Add(Prim);
	}

	void SceneGraph::RemovePrimitive(Ry::ScenePrimitive* Prim)
	{
		Primitives.Remove(Prim);
	}

	void SceneGraph::BeginLoading()
	{
		this->bIsLoading = true;
	}

	void SceneGraph::EndLoading()
	{
		this->bIsLoading = false;
	}

	bool SceneGraph::IsLoading()
	{
		return bIsLoading;
	}

	void SceneGraph::Update(float DeltaTime)
	{
		
	}
	
}

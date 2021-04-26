#include "SceneGraph.h"
#include "ScenePrimitive.h"

namespace Ry
{
	
	SceneGraph::SceneGraph()
	{
		this->bIsLoading = false;
	}

	Ry::ArrayList<ScenePrimitive*>& SceneGraph::GetPrimitives()
	{
		return Primitives;
	}

	void SceneGraph::AddPrimitive(ScenePrimitive* Prim)
	{
		Primitives.Add(Prim);

		OnPrimitiveAdded.Broadcast(Prim);
	}

	void SceneGraph::RemovePrimitive(Ry::ScenePrimitive* Prim)
	{
		Primitives.Remove(Prim);

		OnPrimitiveRemoved.Broadcast(Prim);
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

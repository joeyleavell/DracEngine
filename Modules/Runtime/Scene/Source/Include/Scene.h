#pragma once

#include "World.h"
#include "Entity/Component/TransformComponent.h"
#include "Entity/Component/MeshComponent.h"
#include "SceneGen.h"

namespace Ry
{
	// Scene globals
	SCENE_MODULE extern SharedPtr<World> GlobalWorld;
	SCENE_MODULE extern SharedPtr<Camera> GlobalCamera;

	SCENE_MODULE extern int32 EntityIDCounter;

	template <class T>
	inline SharedPtr<T> MakeComponent()
	{
		return MakeShared(new T);
	}

	SCENE_MODULE SharedPtr<TransformComponent> MakeTransformComponent();
	SCENE_MODULE SharedPtr<TransformComponent> MakeTransformComponent(const Transform& Initial);

	SCENE_MODULE SharedPtr<MeshComponent> MakeMeshComponent();
	SCENE_MODULE SharedPtr<MeshComponent> MakeMeshComponent(SharedPtr<Mesh> RuntimeMesh);

	SCENE_MODULE SharedPtr<World> MakeWorld();
	
	template <class T>
	inline SharedPtr<T> MakeEntity()
	{
		SharedPtr<T> NewEntity = MakeShared(new T);
		NewEntity->SetID(EntityIDCounter++);

		return NewEntity;
	}

	SCENE_MODULE void InitScene();
	void HandleSceneResize(int32 Width, int32 Height);
}

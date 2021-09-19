
#include "Scene.h"

namespace Ry
{

	SharedPtr<World> GlobalWorld;
	SharedPtr<Camera> GlobalCamera;

	
	int32 EntityIDCounter = 0;

	SharedPtr<TransformComponent> MakeTransformComponent()
	{
		return MakeTransformComponent(Ry::Transform{});
	}

	SharedPtr<TransformComponent> MakeTransformComponent(const Transform& Initial)
	{
		SharedPtr<TransformComponent> NewComponent = MakeShared(new TransformComponent);
		NewComponent->SetRelativeTransform(Initial);

		return NewComponent;
	}

	SharedPtr<MeshComponent> MakeMeshComponent()
	{
		return MakeMeshComponent(SharedPtr<Mesh>());
	}

	SharedPtr<MeshComponent> MakeMeshComponent(SharedPtr<Mesh> RuntimeMesh)
	{
		SharedPtr<MeshComponent> NewComponent = MakeShared(new MeshComponent);
		NewComponent->SetRuntimeMesh(RuntimeMesh);

		return NewComponent;
	}

	SharedPtr<World> MakeWorld()
	{
		return MakeShared(new World);
	}


	void InitScene()
	{
		Ry::OnWindowResize.AddFunction(&HandleSceneResize);
	}

	void HandleSceneResize(int32 Width, int32 Height)
	{
		if (GlobalCamera)
		{
			GlobalCamera->resize(Width, Height);
		}
	}
}
#pragma once

#include "Entity/Entity.h"
#include "Scene.h"

namespace Ry
{

	class SCENE_MODULE MeshEntity : public Entity
	{
	public:

		bool ComplexCollision;

		MeshEntity()
		{
			MeshComp = MakeMeshComponent();

			AttachComponent(MeshComp);

			this->ComplexCollision = false;
		}

		void SetMesh(SharedPtr<Mesh2> Mesh)
		{
			MeshComp->SetRuntimeMesh(Mesh);
		}

		void InitPhysics() override
		{
			if (CollisionEnabled)
			{
				Transform World = RootTransform->GetWorldTransform();

				Body = MakeBoxBody(World, Mass, MeshComp->GetRuntimeMesh()->GetBoundingBox());
			}
		}

	private:
		SharedPtr<MeshComponent> MeshComp;
	};
	
}

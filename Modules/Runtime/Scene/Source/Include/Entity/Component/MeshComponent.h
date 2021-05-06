#pragma once

#include "Entity/Component/TransformComponent.h"
#include "Entity/Component/CollisionComponent.h"
#include "Mesh.h"

namespace Ry
{

	template class Ry::SharedPtr<Ry::Mesh>;
	
	class SCENE_MODULE MeshComponent : public TransformComponent
	{
	public:

		bool CollisionEnabled;
		bool ComplexCollision;

		MeshComponent()
		{
			this->CollisionEnabled = true;
			this->ComplexCollision = false;
		}

		~MeshComponent()
		{

		}

		SharedPtr<Mesh> GetRuntimeMesh()
		{
			return RuntimeMesh;
		}

		void SetRuntimeMesh(SharedPtr<Mesh> RTMesh)
		{
			this->RuntimeMesh = RTMesh;
		}

	private:

		SharedPtr<Ry::Mesh> RuntimeMesh;
	};
}

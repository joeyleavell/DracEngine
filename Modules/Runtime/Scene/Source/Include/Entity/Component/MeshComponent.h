#pragma once

#include "Entity/Component/TransformComponent.h"
#include "Entity/Component/CollisionComponent.h"
#include "Mesh2.h"

namespace Ry
{

	template class Ry::SharedPtr<Ry::Mesh2>;
	
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

		SharedPtr<Mesh2> GetRuntimeMesh()
		{
			return RuntimeMesh;
		}

		void SetRuntimeMesh(SharedPtr<Mesh2> RTMesh)
		{
			this->RuntimeMesh = RTMesh;
		}

	private:

		SharedPtr<Ry::Mesh2> RuntimeMesh;
	};
}

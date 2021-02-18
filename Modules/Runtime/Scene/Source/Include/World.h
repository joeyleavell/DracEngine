#pragma once

#include "Core/Core.h"
#include "Entity/Entity.h"
#include "Camera.h"
#include "RenderingEngine.h"
#include <algorithm>

// Physics includes

namespace Ry
{

	class SCENE_MODULE World
	{
	public:

		World();
		virtual ~World();
		void AddEntity(Ry::SharedPtr<Entity> Ent);
		void RemoveEntity(Ry::SharedPtr<Entity> Ent);
		void Update(float Delta);
		void Render();		
		
	private:

		Ry::ArrayList<Ry::SharedPtr<Entity>> Entities;

		// Physics world information
		class btDiscreteDynamicsWorld* PhysicsWorld;
	};
	
}


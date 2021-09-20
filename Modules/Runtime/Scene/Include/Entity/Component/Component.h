#pragma once

#include "Core/Core.h"
#include <vector>
#include "Transform.h"
#include "SceneGen.h"

namespace Ry
{

	class Entity;
	class TransformComponent;

	class SCENE_MODULE Component
	{

	public:

		Component()
		{
			this->Parent = nullptr;
		}
		
		virtual ~Component() = default;

		/**
		 * Called right after a component is attached to something.
		 */
		virtual void InitComponent()
		{
			
		}
		
		virtual void Update(float Delta);

		// void SetOwningEntity(Entity* Parent);
		// Entity* GetOwningEntity() const;

		virtual Transform GetWorldTransform();

		virtual Transform GetRootRelativeTransform();


		TransformComponent* GetParent();
		void SetParent(TransformComponent* Parent);

	private:

		TransformComponent* Parent;
	};
	
}

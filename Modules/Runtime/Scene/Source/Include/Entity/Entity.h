#pragma once

#include "Core/Core.h"
#include "Component/Component.h"
#include "Component/MeshComponent.h"
#include "Component/TransformComponent.h"
#include "Component/CollisionComponent.h"
#include "Core/Globals.h"
#include "Data/ArrayList.h"
#include <vector>

namespace Ry
{

	class SCENE_MODULE Entity
	{
	public:

		bool PhysicsEnabled;
		bool CollisionEnabled;
		float Mass;

		Entity()
		{
			this->Body = nullptr;
			this->CollisionEnabled = true;
			this->PhysicsEnabled = true;
			this->Mass = 0.0f;
		}
		
		virtual ~Entity() = default;
		
		void SetID(int32 ID)
		{
			this->EntityID = ID;
		}

		int32 GetID()
		{
			return EntityID;
		}

		bool CanRender()
		{
			return HasTransformComponent() && HasRenderComponent();
		}

		bool HasTransformComponent()
		{
			return RootTransform.IsValid();
		}

		bool HasRenderComponent()
		{
			return MeshComponents.GetSize() > 0;
		}

		SharedPtr<TransformComponent> GetRootTransform()
		{
			return RootTransform;
		}

		Ry::ArrayList<MeshComponent*>& GetMeshComponents()
		{
			return MeshComponents;
		}

		Ry::ArrayList<CollisionComponent*>& GetCollisionComponents()
		{
			return CollisionComponents;
		}

		SharedPtr<RigidBody> GetBody()
		{
			return Body;
		}

		template<class T>
		void AttachComponent(SharedPtr<T> Component)
		{
			Components.Add(Ry::CastShared<Ry::Component>(Component));

			SharedPtr<TransformComponent> Transform = CastShared<TransformComponent>(Component);

			// ASSUMPTION: Any transform component directly attached to the entity will be a root transform
			if (Transform.IsValid())
			{				
				CORE_ASSERT(!RootTransform.IsValid(), "Root component already attached to entity, cannot replace!")
				RootTransform = Transform;

				Transform->OnHierarchyChanged.AddMemberFunction(this, &Entity::OnChildStructureChanged);
			}

			OnChildStructureChanged();

			Component->InitComponent();
		}

		void DetachComponent(SharedPtr<Ry::Component> Component)
		{
			Components.Remove(Component);

			if(Component == RootTransform)
			{
				TransformComponent* Transform = dynamic_cast<TransformComponent*>(Component.Get());

				RootTransform = nullptr;

				Transform->OnHierarchyChanged.RemoveMemberFunction(this, &Entity::OnChildStructureChanged);
			}

			OnChildStructureChanged();
		}

		virtual void Update(float Delta);		
		virtual void InitPhysics();
		virtual void OnSpawn();

	protected:

		SharedPtr<RigidBody> Body;
		SharedPtr<Ry::TransformComponent> RootTransform;
		Ry::ArrayList<Ry::SharedPtr<Ry::Component>> Components;

	private:

		void OnChildStructureChanged()
		{
			// Check the integrity of our special component storage
			CollisionComponents.Clear();
			MeshComponents.Clear();

			for (SharedPtr<Component> Comp : Components)
			{
				TraverseChildren(Comp.Get());
			}
		}
		
		void TraverseChildren(Component* Root)
		{
			CollisionComponent* Collision = dynamic_cast<CollisionComponent*>(Root);
			MeshComponent* Mesh = dynamic_cast<MeshComponent*>(Root);
			TransformComponent* Transform = dynamic_cast<TransformComponent*>(Root);

			if (Collision)
			{
				CollisionComponents.Add(Collision);
			}
			else if (Mesh)
			{
				MeshComponents.Add(Mesh);
			}

			if (Transform)
			{
				for(SharedPtr<Component> Child : Transform->GetChildren())
				{
					TraverseChildren(Child.Get());
				}				
			}
		}

		
		int32 EntityID;

		// Keep track of special components
		Ry::ArrayList<Ry::CollisionComponent*> CollisionComponents;
		Ry::ArrayList<Ry::MeshComponent*> MeshComponents;
	};
	
}

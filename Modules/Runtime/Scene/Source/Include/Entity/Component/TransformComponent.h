#pragma once

#include "Entity/Component/Component.h"
#include "Transform.h"
#include "Data/ArrayList.h"

namespace Ry
{

	class SCENE_MODULE TransformComponent : public Component
	{
	public:

		MulticastDelegate<> OnHierarchyChanged;

		TransformComponent() = default;

		~TransformComponent() = default;
		
		Transform GetWorldTransform() override
		{
			return RelativeTransform * Component::GetWorldTransform();
		}

		Transform GetRootRelativeTransform() override
		{
			return RelativeTransform * Component::GetRootRelativeTransform();
		}

		Transform& GetRelativeTransform()
		{
			return RelativeTransform;
		}

		void SetRelativeTransform(const Transform& Transform)
		{
			this->RelativeTransform = Transform;
		}

		
		void AttachComponent(SharedPtr<Component> Child)
		{
			Children.Add(Child);
			Child->SetParent(this);

			// Check if this is a transform component
			SharedPtr<TransformComponent> TransformChild = Ry::CastShared<TransformComponent>(Child);

			if(TransformChild)
			{
				TransformChild->OnHierarchyChanged.AddMemberFunction(this, &TransformComponent::OnChildHierarchyChanged);
			}

			OnHierarchyChanged.Broadcast();

			Child->InitComponent();
		}

		void DetachComponent(SharedPtr<Component> Child)
		{
			if(Children.Contains(Child))
			{
				// Remove ourselves from their event listener
				OnHierarchyChanged.RemoveMemberFunction(this, &TransformComponent::OnChildHierarchyChanged);

				// Remove us as their parent
				Child->SetParent(nullptr);
			}
		}

		Ry::ArrayList<SharedPtr<Component>>& GetChildren()
		{
			return Children;
		}
		
	private:

		void OnChildHierarchyChanged()
		{
			OnHierarchyChanged.Broadcast();
		}

		Ry::ArrayList<SharedPtr<Component>> Children;
		Transform RelativeTransform;
	};
	
}

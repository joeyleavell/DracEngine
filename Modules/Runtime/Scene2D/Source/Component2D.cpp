#include "Component2D.h"

namespace Ry
{
	Component2D::Component2D(Entity2D* Owner)
	{
		this->Owner = Owner;
		this->bCanUpdate = false;
		this->bUpdateEnabled = false;
	}

	void Component2D::FireUpdate(float Delta)
	{
		if(bUpdateEnabled)
		{
			Update(Delta);
		}
	}

	bool Component2D::CanEverUpdate() const
	{
		return bCanUpdate;
	}

	bool Component2D::IsUpdateEnabled() const
	{
		return bUpdateEnabled;
	}

	Transform2DComponent::Transform2DComponent(Entity2D* Owner):
	Component2D(Owner)
	{
		
	}

	Transform2D& Transform2DComponent::GetRelativeTransform()
	{
		return Transform;
	}

	Vector2& Transform2DComponent::GetRelativePos()
	{
		return Transform.Position;
	}

	Vector2& Transform2DComponent::GetRelativeScale()
	{
		return Transform.Scale;
	}

	float& Transform2DComponent::GetRelativeRotation()
	{
		return Transform.Rotation;
	}

	Transform2D Transform2DComponent::GetWorldTransform()
	{
		// todo: correctly calc world transform
		return GetRelativeTransform();
	}

	Vector2 Transform2DComponent::GetWorldPos()
	{
		return GetRelativePos();
	}

	Vector2 Transform2DComponent::GetWorldScale()
	{
		return GetWorldScale();
	}

	float Transform2DComponent::GetWorldRotation()
	{
		return GetWorldRotation();
	}

	void Transform2DComponent::AttachTo(const SharedPtr<Transform2DComponent>& Parent)
	{
		this->ParentComponent = Parent;
	}

	Primitive2DComponent::Primitive2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size, const Vector2& Origin):
	Transform2DComponent(Owner)
	{
		this->Mobility = Mobility;
		this->Size = Size;
		this->Origin = Origin;
	}

	void Primitive2DComponent::DrawPrimitive()
	{
		if(Primitive.IsValid())
		{
			// Calculate world transform
			Transform2D WorldTransform = GetWorldTransform();

			// Draw primitive
			Primitive->Draw(WorldTransform.Position.x, WorldTransform.Position.y,
				Size.x * WorldTransform.Scale.x, Size.y * WorldTransform.Scale.y,
				Origin.x, Origin.y,
				WorldTransform.Rotation);
		}
	}

	Ry::Vector2 Primitive2DComponent::GetSize()
	{
		return Size;
	}

	Ry::Vector2 Primitive2DComponent::GetOrigin()
	{
		return Origin;
	}

	Ry::SharedPtr<Ry::ScenePrimitive2D> Primitive2DComponent::GetPrimitive()
	{
		return Primitive;
	}

	Texture2DComponent::Texture2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size, const Vector2& Origin, TextureRegion Texture):
	Primitive2DComponent(Owner, Mobility, Size, Origin)
	{
		Primitive = MakeShared(new TextureScenePrimitive{ Mobility, Texture });
	}
	
}

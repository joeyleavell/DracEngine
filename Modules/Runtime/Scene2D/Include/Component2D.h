#pragma once

#include "Core/Delegate.h"
#include "Vector.h"
#include "Scene2D.h"

namespace Ry
{

	class Entity2D;

	struct SCENE2D_MODULE TickFunction
	{
		Delegate<void, float> TickDelegate;
	};

	struct SCENE2D_MODULE Transform2D
	{
		Ry::Vector2 Position{0.0f, 0.0f};
		Ry::Vector2 Scale{1.0f, 1.0f};
		float Rotation {0.0f};


		Ry::Matrix3 AsMatrix() const
		{
			Ry::Matrix3 T = Ry::Translation2DMatrix(Position.x, Position.y);
			Ry::Matrix3 R = Ry::Rotation2DMatrix(Rotation);
			Ry::Matrix3 S = Ry::Scale2DMatrix(Scale.x, Scale.y);

			return T * R * S;
		}

		Ry::Matrix3 Compose(const Transform2D& Other)
		{
			return Other.AsMatrix() * AsMatrix();
		}

		friend Matrix3 ComposeMatrix(const Matrix3& Left, const Transform2D& Transform);
	};

	class SCENE2D_MODULE Component2D
	{
	public:

		Component2D(Entity2D* Owner);

		void FireUpdate(float Delta);
		void Update(float Delta) {};

		bool CanEverUpdate() const;
		bool IsUpdateEnabled() const;

	protected:

		// Entity that owns this component
		Entity2D* Owner;
		
		bool bCanUpdate;
		bool bUpdateEnabled;

	};

	class SCENE2D_MODULE Transform2DComponent : public Component2D
	{
	public:

		Transform2DComponent(Entity2D* Owner);

		Transform2D& GetRelativeTransform();
		Vector2& GetRelativePos();
		Vector2& GetRelativeScale();
		float& GetRelativeRotation();

		Ry::Matrix3 GetWorldTransform();
		Vector2 GetWorldPos();
		Vector2 GetWorldScale();
		float GetWorldRotation();

		void AttachTo(const SharedPtr<Transform2DComponent>& Parent);

	private:
		SharedPtr<Transform2DComponent> ParentComponent;
		Transform2D Transform;
	};

	class SCENE2D_MODULE Primitive2DComponent : public Transform2DComponent
	{
	public:

		Primitive2DComponent(Entity2D* Owner, Ry::PrimitiveMobility Mobility, const Vector2& Size = {0.0f, 0.0f}, const Vector2& Origin = { 0.0f, 0.0f });

		void DrawPrimitive();

		Ry::Vector2 GetSize();
		Ry::Vector2 GetOrigin();

		Ry::SharedPtr<Ry::ScenePrimitive2D> GetPrimitive();
		
	protected:

		PrimitiveMobility Mobility;

		Ry::Vector2 Size;
		Ry::Vector2 Origin;

		Ry::SharedPtr<Ry::ScenePrimitive2D> Primitive;
		
	};

	class SCENE2D_MODULE Texture2DComponent : public Primitive2DComponent
	{
	public:

		Texture2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size = { 0.0f, 0.0f }, const Vector2& Origin = { 0.0f, 0.0f }, TextureRegion Texture = {});
	};

	class SCENE2D_MODULE Animation2DComponent : public Primitive2DComponent
	{
	public:

		Animation2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size = { 0.0f, 0.0f }, const Vector2& Origin = { 0.0f, 0.0f }, SharedPtr<Animation> Anim = {});
	};

}

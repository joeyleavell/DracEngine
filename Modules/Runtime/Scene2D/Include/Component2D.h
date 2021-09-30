#pragma once

#include "Core/Delegate.h"
#include "Vector.h"
#include "Scene2D.h"
#include "Transform.h"

class b2Body;

namespace Ry
{

	enum class Physics2DType
	{
		Static,
		Dynamic
	};

	struct PhysicsMaterial2D
	{
		Physics2DType Type;
		float Density = 1.0f;
		float Friction = 0.3f;
		float Restitution = 0.3f;

		bool bFixedRotation = false;
	};

	class World2D;
	class Entity2D;

	struct SCENE2D_MODULE TickFunction
	{
		Delegate<void, float> TickDelegate;
	};

	class SCENE2D_MODULE Component2D
	{
	public:

		Component2D(Entity2D* Owner);

		void FireUpdate(float Delta);
		virtual void Update(float Delta) {};

		bool CanEverUpdate() const;
		bool IsUpdateEnabled() const;

		World2D* GetWorld() const;
		Entity2D* GetOwner() const;

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

		void SetRelativeTransform(const Transform2D& Transform);

		Transform2D& GetRelativeTransform();
		Vector2& GetRelativePos();
		Vector2& GetRelativeScale();
		float& GetRelativeRotation();

		Ry::Transform2D GetWorldTransform();
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

	class SCENE2D_MODULE Physics2DComponent : public Transform2DComponent
	{
	public:

		Physics2DComponent(Entity2D* Owner, PhysicsMaterial2D Mat);
		~Physics2DComponent();

		void Update(float Delta) override;

		b2Body* GetBody();

		virtual void CreatePhysicsState() = 0;
		void DestroyPhysicsState();

		virtual void OnBeginOverlap(Physics2DComponent* Other);
		
		virtual void OnEndOverlap(Physics2DComponent* Other);

		void ApplyForceToCenter(float X, float Y);

	protected:

		PhysicsMaterial2D Mat;
		b2Body* Body;
	};

	class SCENE2D_MODULE Box2DComponent : public Physics2DComponent
	{
	public:

		Box2DComponent(Entity2D* Owner, PhysicsMaterial2D Mat, float Width, float Height);

		void CreatePhysicsState() override;

	private:

		float Width;
		float Height;

	};

}

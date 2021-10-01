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

		float LinearDamping = 8.0f;
		float AngularDamping = 1.0f;
		bool bIsBullet = false;
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

		MulticastDelegate<> OnRenderStateDirty;

		Primitive2DComponent(Entity2D* Owner, Ry::PrimitiveMobility Mobility, const Vector2& Size = {0.0f, 0.0f}, const Vector2& Origin = { 0.0f, 0.0f });

		void DrawPrimitive();

		Ry::Vector2 GetSize();
		Ry::Vector2 GetOrigin();

		Ry::SharedPtr<Ry::ScenePrimitive2D> GetPrimitive();

		void SetVisibility(bool bVisible);
		bool IsVisible() const;
		
	protected:

		bool bVisible;

		PrimitiveMobility Mobility;

		Ry::Vector2 Size;
		Ry::Vector2 Origin;

		Ry::SharedPtr<Ry::ScenePrimitive2D> Primitive;
		
	};

	class SCENE2D_MODULE Text2DComponent : public Primitive2DComponent
	{
	public:

		Text2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size = { 0.0f, 0.0f }, const Vector2& Origin = { 0.0f, 0.0f }, Ry::String Text = "", BitmapFont* Font = nullptr, int32 Layer = 0);

	};

	class SCENE2D_MODULE Texture2DComponent : public Primitive2DComponent
	{
	public:

		Texture2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size = { 0.0f, 0.0f }, const Vector2& Origin = { 0.0f, 0.0f }, TextureRegion Texture = {});
	};

	class SCENE2D_MODULE RectComponent : public Primitive2DComponent
	{
	public:

		RectComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size = { 0.0f, 0.0f }, const Vector2& Origin = { 0.0f, 0.0f }, Color RectColor = WHITE);
	};

	class SCENE2D_MODULE Animation2DComponent : public Primitive2DComponent
	{
	public:

		Animation2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size = { 0.0f, 0.0f }, const Vector2& Origin = { 0.0f, 0.0f }, SharedPtr<Animation> Anim = {});

		void Pause(int32 AtFrame = 0);
		void Play();
		void SetAnimation(SharedPtr<Animation> Anim);
		void SetDelay(float Delay);

	};


	struct AnimPair
	{
		Ry::String Name;
		Ry::SharedPtr<Animation> Anim;

		AnimPair(Ry::String Name, Ry::SharedPtr<Animation> A)
		{
			this->Name = Name;
			Anim = A;
		}
	};

	class SCENE2D_MODULE AnimationStateComponent : public Component2D
	{
	public:

		AnimationStateComponent(Entity2D* Owner, Ry::SharedPtr<Animation2DComponent> AnimComp);

		void SetAnims(std::initializer_list<AnimPair> Pairs);

		void Pause(int32 AtFrame = 0);
		void Play();
		void Play(Ry::String Name);

		void SetAnimDelay(float Delay);

	private:

		Ry::String CurrentAnim;
		bool bPaused;
		Ry::OAHashMap<Ry::String, SharedPtr<Ry::Animation>> Anims;
		SharedPtr<Animation2DComponent> AnimComp;

	};

	class SCENE2D_MODULE Physics2DComponent : public Transform2DComponent
	{
	public:

		float DragCoefficient = 7.0f;

		Physics2DComponent(Entity2D* Owner, PhysicsMaterial2D Mat);
		~Physics2DComponent();

		void Update(float Delta) override;

		b2Body* GetBody();

		virtual void CreatePhysicsState() = 0;
		void DestroyPhysicsState();

		virtual void OnBeginOverlap(Physics2DComponent* Other);
		
		virtual void OnEndOverlap(Physics2DComponent* Other);

		void ApplyForceToCenter(float X, float Y);
		void SetLinearVelocity(float X, float Y);
		void ApplyDragForce(float C);

		float GetSpeed() const;
		Vector2 GetVelocity() const;

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

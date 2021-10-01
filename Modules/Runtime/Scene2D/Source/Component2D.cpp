#include "Component2D.h"
#include "Entity2D.h"
#include <box2d/b2_world.h>
#include "box2d/b2_body.h"
#include "World2D.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_math.h"

namespace Ry
{

	Matrix3 ComposeMatrix(const Matrix3& Left, const Transform2D& Transform)
	{
		return Left * Transform.AsMatrix();
	}
	
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

	World2D* Component2D::GetWorld() const
	{
		return Owner->GetWorld();
	}

	Entity2D* Component2D::GetOwner() const
	{
		return Owner;
	}

	Transform2DComponent::Transform2DComponent(Entity2D* Owner):
	Component2D(Owner)
	{
		
	}

	void Transform2DComponent::SetRelativeTransform(const Transform2D& Transform)
	{
		this->Transform = Transform;
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

	Ry::Transform2D Transform2DComponent::GetWorldTransform()
	{
		Ry::Transform2D ParentTransform;
		if(ParentComponent)
		{
			ParentTransform = ParentComponent->GetWorldTransform();
		}
		else if(Owner->GetRootComponent() != this)
		{
			ParentTransform = Owner->GetRootComponent()->GetWorldTransform();
		}

		return ParentTransform.Compose(GetRelativeTransform());
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
		this->bVisible = true;
	}

	void Primitive2DComponent::DrawPrimitive()
	{
		if(Primitive.IsValid())
		{
			// Primitive's grouping needs to be updating
			if(Primitive->bPrimitiveStateDirty)
			{
				Primitive->bPrimitiveStateDirty = false;
				OnRenderStateDirty.Broadcast();
			}
			
			// Calculate world transform
			Ry::Transform2D WorldTransform = GetWorldTransform();

			// Draw primitive
			Primitive->Draw(WorldTransform.AsMatrix(), Origin);
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

	void Primitive2DComponent::SetVisibility(bool bVisible)
	{
		this->bVisible = bVisible;
		OnRenderStateDirty.Broadcast();
	}

	bool Primitive2DComponent::IsVisible() const
	{
		return bVisible;
	}

	Text2DComponent::Text2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size,
		const Vector2& Origin, Ry::String Text, BitmapFont* Font, int32 Layer):
	Primitive2DComponent(Owner, Mobility, Size, Origin)
	{
		Primitive = MakeShared(new TextScenePrimitive { Mobility, Size, Text, Font});
		Primitive->SetLayer(Layer);
	}

	Texture2DComponent::Texture2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size, const Vector2& Origin, TextureRegion Texture):
	Primitive2DComponent(Owner, Mobility, Size, Origin)
	{
		Primitive = MakeShared(new TextureScenePrimitive{ Mobility, Size, Texture});
	}

	RectComponent::RectComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size,
		const Vector2& Origin, Color RectColor):
	Primitive2DComponent(Owner, Mobility, Size, Origin)
	{
		Primitive = MakeShared(new RectScenePrimitive{ Mobility, Size, RectColor });
	}
	

	AnimationStateComponent::AnimationStateComponent(Entity2D* Owner, Ry::SharedPtr<Animation2DComponent> AnimComp):
	Component2D(Owner)
	{
		this->AnimComp = AnimComp;
	}

	void AnimationStateComponent::SetAnims(std::initializer_list<AnimPair> Pairs)
	{
		for (const AnimPair& Pair : Pairs)
		{
			this->Anims.Insert(Pair.Name, Pair.Anim);
		}


		if (Pairs.size() > 0)
		{
			Play(Pairs.begin()->Name);
		}
		else
		{
			std::cerr << "No animations passed into animation state component" << std::endl;
		}
	}

	void AnimationStateComponent::Pause(int32 AtFrame)
	{
		if (AnimComp.IsValid())
		{
			AnimComp->Pause(AtFrame);
		}
	}

	void AnimationStateComponent::Play()
	{
		if(AnimComp.IsValid())
		{
			AnimComp->Play();
		}
	}

	void AnimationStateComponent::Play(Ry::String Name)
	{
		CurrentAnim = Name;

		SharedPtr<Animation> Anim = Anims.Get(Name);
		AnimComp->SetAnimation(Anim);
	}

	void AnimationStateComponent::SetAnimDelay(float Delay)
	{
		AnimComp->SetDelay(Delay);
	}

	Animation2DComponent::Animation2DComponent(Entity2D* Owner, PrimitiveMobility Mobility, const Vector2& Size,
	                                           const Vector2& Origin, SharedPtr<Animation> Anim):
	Primitive2DComponent(Owner, Mobility, Size, Origin)
	{
		Primitive = MakeShared(new AnimationScenePrimitive{Mobility, Size, Anim});
	}

	void Animation2DComponent::Pause(int32 AtFrame)
	{
		if(SharedPtr<AnimationScenePrimitive> Anim = CastShared<AnimationScenePrimitive>(Primitive))
		{
			Anim->Pause(AtFrame);
		}
	}

	void Animation2DComponent::Play()
	{
		if (SharedPtr<AnimationScenePrimitive> Anim = CastShared<AnimationScenePrimitive>(Primitive))
		{
			Anim->Play();
		}

	}

	void Animation2DComponent::SetAnimation(SharedPtr<Animation> Anim)
	{
		if (SharedPtr<AnimationScenePrimitive> A = CastShared<AnimationScenePrimitive>(Primitive))
		{
			A->SetAnim(Anim);
			OnRenderStateDirty.Broadcast();
		}
	}

	void Animation2DComponent::SetDelay(float Delay)
	{
		if (SharedPtr<AnimationScenePrimitive> A = CastShared<AnimationScenePrimitive>(Primitive))
		{
			A->SetDelay(Delay);
		}
	}

	Physics2DComponent::Physics2DComponent(Entity2D* Owner, PhysicsMaterial2D Mat):
	Transform2DComponent(Owner)
	{
		bCanUpdate = true;
		bUpdateEnabled = true;

		this->Mat = Mat;
		this->Body = nullptr;
	}

	Physics2DComponent::~Physics2DComponent()
	{
		DestroyPhysicsState();
	}

	void Physics2DComponent::Update(float Delta)
	{
		if(Body)
		{
			// Update transform
			float XPixels = MetersToPixels(Body->GetPosition().x);
			float YPixels = MetersToPixels(Body->GetPosition().y);

			GetRelativePos().x = XPixels;
			GetRelativePos().y = YPixels;
			GetRelativeRotation() = RAD_TO_DEG(Body->GetAngle());
		}
	}

	b2Body* Physics2DComponent::GetBody()
	{
		return Body;
	}

	void Physics2DComponent::DestroyPhysicsState()
	{
		// Remove body from world
		if(Body)
		{
			if (b2World* PhysWorld = Owner->GetWorld()->GetPhysicsWorld())
			{
				PhysWorld->DestroyBody(Body);
				Body = nullptr;
			}
		}
	}

	void Physics2DComponent::OnBeginOverlap(Physics2DComponent* Other)
	{
		GetOwner()->OnBeginOverlap(Other->GetOwner(), Other, this);
	}

	void Physics2DComponent::OnEndOverlap(Physics2DComponent* Other)
	{
		GetOwner()->OnEndOverlap(Other->GetOwner(), Other, this);
	}

	void Physics2DComponent::ApplyForceToCenter(float X, float Y)
	{
		Body->ApplyForceToCenter(b2Vec2{ X, Y }, true);
	}

	void Physics2DComponent::ApplyDragForce(float C)
	{
		b2Vec2 Vel = Body->GetLinearVelocity();
		
		float Mag = Vel.Length();
		Vel.Normalize();

		float DragMag = C * Mag * Mag;
		b2Vec2 Drag = DragMag * -Vel;
		
		Body->ApplyForceToCenter(Drag, true);
	}

	float Physics2DComponent::GetSpeed() const
	{
		return MetersToPixels(Body->GetLinearVelocity().Length());
	}

	Vector2 Physics2DComponent::GetVelocity() const
	{
		return Ry::Vector2(MetersToPixels(Body->GetLinearVelocity().x), MetersToPixels(Body->GetLinearVelocity().y));
	}

	void Physics2DComponent::SetLinearVelocity(float X, float Y)
	{
		Body->SetLinearVelocity(b2Vec2{ PixelsToMeters(X), PixelsToMeters(Y) });
	}

	Box2DComponent::Box2DComponent(Entity2D* Owner, PhysicsMaterial2D Mat, float Width, float Height):
	Physics2DComponent(Owner, Mat)
	{
		this->Width = Width;
		this->Height = Height;
	}

	void Box2DComponent::CreatePhysicsState()
	{
		b2World* ParentWorld = GetWorld()->GetPhysicsWorld();
		
		Transform2D WorldPosition = GetWorldTransform();
		
		float XMeters = PixelsToMeters(WorldPosition.Position.x);
		float YMeters = PixelsToMeters(WorldPosition.Position.y);

		b2BodyDef BodyDef;
		BodyDef.type = Mat.Type == Physics2DType::Dynamic ? b2_dynamicBody : b2_staticBody;
		BodyDef.position.Set(XMeters, YMeters);
		BodyDef.fixedRotation = Mat.bFixedRotation;
		BodyDef.linearDamping = Mat.LinearDamping;
		BodyDef.angularDamping = Mat.AngularDamping;
		BodyDef.bullet = Mat.bIsBullet;

		Body = ParentWorld->CreateBody(&BodyDef);

		float WMeters = PixelsToMeters(Width / 2);
		float HMeters = PixelsToMeters(Height / 2);
		b2PolygonShape BoxShape{};
		BoxShape.SetAsBox(WMeters, HMeters);

		b2FixtureDef FixtureDef{};
		FixtureDef.shape = &BoxShape;
		FixtureDef.density = Mat.Density;
		FixtureDef.friction = Mat.Friction;
		FixtureDef.restitution = Mat.Restitution;
		FixtureDef.userData.pointer = (uintptr_t) this;

		Body->CreateFixture(&FixtureDef);
		
	}
	
}

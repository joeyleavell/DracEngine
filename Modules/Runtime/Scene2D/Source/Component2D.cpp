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
		return GetRelativeScale();
	}

	float Transform2DComponent::GetWorldRotation()
	{
		return GetRelativeRotation();
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

	ParticleEmitter2DComponent::ParticleEmitter2DComponent(Entity2D* Owner, PrimitiveMobility Mobility,
		const ParticleEmitter& EmitterDef, int32 Layer):
	Primitive2DComponent(Owner, Mobility, Ry::Vector2{}, Ry::Vector2{})
	{
		EmitterPrimitive = MakeShared(new ParticleEmitterPrimitive{ Mobility, EmitterDef.Parent});
		Primitive = CastShared<ScenePrimitive2D>(EmitterPrimitive);
		Primitive->SetLayer(Layer);

		this->Emitter = EmitterDef;

		SpawnDelta = 1.0f / EmitterDef.ParticlesPerSecond;
		bCanUpdate = true;
		bUpdateEnabled = true;
	}

	void ParticleEmitter2DComponent::Update(float Delta)
	{
		Primitive2DComponent::Update(Delta);

		if(LastPart >= SpawnDelta && Emitter.Defs.GetSize() > 0)
		{
			// choose a particle def at random
			int32 RandIndex = rand() % Emitter.Defs.GetSize();
			ParticleDef& Def = Emitter.Defs[RandIndex];

			// Update dynamic data further down
			Ry::SharedPtr<Particle> NewPart = MakeShared(new Particle{});
			NewPart->Width = Emitter.ParticleBaseWidth;
			NewPart->Height = Emitter.ParticleBaseHeight;
			NewPart->Transform.Position = GetWorldTransform().Position;
			NewPart->Tint = Def.StartingTint;

			float RandX = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
			float RandY = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - 0.5f) * 2.0f;
			float RandSpeed = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
			RandSpeed = RandSpeed * (Emitter.MinSpeed) + (1.0f - RandSpeed) * Emitter.MaxSpeed;

			Ry::Vector2 Vel(RandX, RandY);
			normalize(Vel);
			Vel *= RandSpeed;

			NewPart->Vx = Vel.x;
			NewPart->Vy = Vel.y;
			NewPart->Opacity = 1.0f;

			Particles.Add(NewPart);
			EmitterPrimitive->AddParticle(NewPart);

			LastPart = 0.0f;
		}
		else
		{
			LastPart += Delta;
		}

		for(SharedPtr<Ry::Particle> Part : Particles)
		{
			Part->Transform.Position.x += Part->Vx * Delta;
			Part->Transform.Position.y += Part->Vy * Delta;
			Part->TimeAlive += Delta;

			// Try kill particle
			if(Part->TimeAlive >= Emitter.TimeAlive)
			{
				EmitterPrimitive->RemoveParticle(Part);
				Particles.Remove(Part);
			}
		}
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
		if(Name != CurrentAnim)
		{
			CurrentAnim = Name;

			SharedPtr<Animation> Anim = Anims.Get(Name);
			AnimComp->SetAnimation(Anim);
		}
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

	Physics2DComponent::Physics2DComponent(Entity2D* Owner, PhysicsMaterial2D Mat, bool bControl):
	Transform2DComponent(Owner)
	{
		bCanUpdate = true;
		bUpdateEnabled = true;

		this->Mat = Mat;
		this->Body = nullptr;
		this->bAuthoritative = bControl;
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

			if(!Mat.bFixedRotation)
			{
				GetRelativeRotation() = RAD_TO_DEG(Body->GetAngle());
			}
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

	void Physics2DComponent::OnBeginOverlap(Physics2DComponent* Other, b2Fixture* ThisFixture)
	{
		// Find b2Fixture->Shape mapping		
		GetOwner()->OnBeginOverlap(Other->GetOwner(), Other, this, B2ToGenericShapes.Contains(ThisFixture) ? B2ToGenericShapes.Get(ThisFixture) : nullptr);
	}

	void Physics2DComponent::OnEndOverlap(Physics2DComponent* Other, b2Fixture* ThisFixture)
	{
		GetOwner()->OnEndOverlap(Other->GetOwner(), Other, this, B2ToGenericShapes.Contains(ThisFixture) ? B2ToGenericShapes.Get(ThisFixture) : nullptr);
	}

	void Physics2DComponent::ApplyForceToCenter(float X, float Y)
	{
		Body->ApplyForceToCenter(b2Vec2{ X, Y }, true);
	}

	void Physics2DComponent::ApplyImpulseToCenter(float X, float Y)
	{
		Body->ApplyLinearImpulseToCenter(b2Vec2{ X, Y }, true);
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

	void Physics2DComponent::SetPosition(float X, float Y)
	{
		float XMeters = PixelsToMeters(X);
		float YMeters = PixelsToMeters(Y);

		Body->SetTransform(b2Vec2{ XMeters, YMeters }, Body->GetAngle());
	}

	void Physics2DComponent::SetPosition(Ry::Vector2 Vec)
	{
		SetPosition(Vec.x, Vec.y);
	}

	void Physics2DComponent::SetBodyType(Physics2DType Type)
	{
		if (Type == Physics2DType::Dynamic)
			Body->SetType(b2_dynamicBody);
		if (Type == Physics2DType::Static)
			Body->SetType(b2_staticBody);
		if (Type == Physics2DType::Kinetmatic)
			Body->SetType(b2_kinematicBody);

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
		if(Body)
		{
 			Body->SetLinearVelocity(b2Vec2{ PixelsToMeters(X), PixelsToMeters(Y) });
		}
		else
		{
			bLastVelocitySet = true;
			LastVelocity = Ry::Vector2(X, Y);
		}
	}

	void Physics2DComponent::SetLinearVelocity(Ry::Vector2 Vec)
	{
		SetLinearVelocity(Vec.x, Vec.y);
	}

	Box2DComponent::Box2DComponent(Entity2D* Owner, PhysicsMaterial2D Mat):
	Physics2DComponent(Owner, Mat)
	{
	}

	void Box2DComponent::CreatePhysicsState()
	{
		b2World* ParentWorld = GetWorld()->GetPhysicsWorld();
		
		Transform2D WorldPosition = GetWorldTransform();
		
		float XMeters = PixelsToMeters(WorldPosition.Position.x);
		float YMeters = PixelsToMeters(WorldPosition.Position.y);

		b2BodyDef BodyDef;
		if (Mat.Type == Physics2DType::Dynamic)
			BodyDef.type = b2_dynamicBody;
		if (Mat.Type == Physics2DType::Static)
			BodyDef.type = b2_staticBody;
		if (Mat.Type == Physics2DType::Kinetmatic)
			BodyDef.type = b2_kinematicBody;

		BodyDef.position.Set(XMeters, YMeters);
		BodyDef.fixedRotation = Mat.bFixedRotation;
		BodyDef.linearDamping = Mat.LinearDamping;
		BodyDef.angularDamping = Mat.AngularDamping;
		BodyDef.bullet = Mat.bIsBullet;

		Body = ParentWorld->CreateBody(&BodyDef);

		for(PhysicsShape* Shape : Mat.PhysicsShapes)
		{
			b2Shape* B2Shape = nullptr;

			PhysicsBoxShape* AsBox = dynamic_cast<PhysicsBoxShape*>(Shape);

			if(AsBox)
			{
				float XMeters = PixelsToMeters(AsBox->X);
				float YMeters = PixelsToMeters(AsBox->Y);
				float WMeters = PixelsToMeters(AsBox->Width / 2);
				float HMeters = PixelsToMeters(AsBox->Height / 2);
				b2PolygonShape* B2BoxShape = new b2PolygonShape;
				B2BoxShape->SetAsBox(WMeters, HMeters, b2Vec2{ XMeters, YMeters }, 0.0f);
				B2Shape = B2BoxShape;
			}

			if (B2Shape)
			{
				b2FixtureDef FixtureDef{};
				FixtureDef.shape = B2Shape;
				FixtureDef.density = Shape->Density;
				FixtureDef.friction = Shape->Friction;
				FixtureDef.restitution = Shape->Restitution;
				FixtureDef.isSensor = Shape->bSensor;
				FixtureDef.userData.pointer = (uintptr_t)this;

				b2Fixture* Fix = Body->CreateFixture(&FixtureDef);
				B2ToGenericShapes.Insert(Fix, Shape);
			}
		}

		if(bLastVelocitySet)
		{
			SetLinearVelocity(LastVelocity);
			bLastVelocitySet = false;
		}


		
	}
	
}

#include "World2D.h"

#include "box2d/b2_world.h"
#include "box2d/b2_contact.h"
#include "SwapChain.h"

namespace Ry
{

	class Box2DContactListener : public b2ContactListener
	{
	public:
		Delegate<void, b2Contact*> OnBeginWorldContact;
		Delegate<void, b2Contact*> OnEndWorldContact;

		Box2DContactListener()
		{
		}

		void BeginContact(b2Contact* Contact) override
		{
			OnBeginWorldContact.Execute(Contact);
		}

		void EndContact(b2Contact* Contact) override
		{
			OnEndWorldContact.Execute(Contact);
		}
	};

	World2D::World2D(SwapChain* SC, Ry::Vector2 Gravity)
	{
		WorldScene = new Scene2D(SC);

		// Instantiate physics world
		PhysicsWorld = new b2World(b2Vec2{Gravity.x, Gravity.y});

		// Setup contact listener
		Box2DContactListener* B2ContactListener = new Box2DContactListener;
		B2ContactListener->OnBeginWorldContact.BindMemberFunction(this, &World2D::OnBeginContact);
		B2ContactListener->OnEndWorldContact.BindMemberFunction(this, &World2D::OnEndContact);
		PhysicsWorld->SetContactListener(B2ContactListener);
		ContactListener = B2ContactListener;

		this->Camera = new Camera2D { (float) SC->GetSwapChainWidth(), (float)SC->GetSwapChainHeight() };
		this->CameraController = nullptr;
	}

	World2D::~World2D()
	{
		delete PhysicsWorld;
		delete ContactListener;
	}

	void World2D::Resize(int32 Width, int32 Height)
	{
		Camera->Resize(Width, Height);
		WorldScene->Resize(Width, Height);
	}

	void World2D::SetCameraController(Ry::CameraController2D* Controller)
	{
		this->CameraController = Controller;
	}

	void World2D::AddEntity(const Ry::SharedPtr<Entity2D> Ent)
	{
		Entities.Add(Ent);

		// Initial draw
		Ent->UpdatePrimitives();

		// Bind on entity dirty
		Ent->OnRenderStateDirty.AddMemberFunction(this, &World2D::OnEntityDirty);

		OnEntityDirty(Ent.Get());
	}

	void World2D::RemoveEntity(const Ry::SharedPtr<Entity2D> Ent)
	{
		// Remove entity dirty binding
		Ent->OnRenderStateDirty.RemoveMemberFunction(this, &World2D::OnEntityDirty);

		Ent->SetVisibility(false);
		OnEntityDirty(Ent.Get());

		Ent->End();
		
		Entities.Remove(Ent);
	}

	void World2D::Update(float Delta)
	{
		// Physics update
		int32 VelocityIterations = 8;
		int32 PositionIterations = 3;
		PhysicsWorld->Step(Delta, VelocityIterations, PositionIterations);

		for(SharedPtr<Entity2D> Ent : Entities)
		{
			Ent->FireUpdate(Delta);

			if(Ent->IsPendingKill())
			{
				KillEnts.Add(Ent);
			}
		}

		// Remove all killed entities
		if(KillEnts.GetSize() > 0)
		{
			for(SharedPtr<Entity2D> Ent : KillEnts)
			{
				RemoveEntity(Ent);
			}
			
			KillEnts.Clear();
		}

		WorldScene->Update(Delta);

		// Update camera
		if (CameraController)
		{
			CameraController->Update(Delta, *Camera);
		}
		Camera->Update();
	}

	void World2D::UpdateStaticGeometry()
	{
		WorldScene->UpdateStatic();
	}

	void World2D::Draw()
	{
		// Draw the scene
		WorldScene->Render(*Camera);
	}

	b2World* World2D::GetPhysicsWorld() const
	{
		return PhysicsWorld;
	}

	void World2D::OnBeginContact(b2Contact* Contact)
	{
		Physics2DComponent* A = (Physics2DComponent*) Contact->GetFixtureA()->GetUserData().pointer;
		Physics2DComponent* B = (Physics2DComponent*) Contact->GetFixtureB()->GetUserData().pointer;

		if(A)
		{
			A->OnBeginOverlap(B);
		}

		if(B)
		{
			B->OnBeginOverlap(A);
		}
	}

	void World2D::OnEndContact(b2Contact* Contact)
	{
		Physics2DComponent* A = (Physics2DComponent*)Contact->GetFixtureA()->GetUserData().pointer;
		Physics2DComponent* B = (Physics2DComponent*)Contact->GetFixtureB()->GetUserData().pointer;

		if (A)
		{
			A->OnEndOverlap(B);
		}

		if (B)
		{
			B->OnEndOverlap(A);
		}
	}

	void World2D::OnEntityDirty(Entity2D* Ent)
	{
		// Update scene primitives
		auto ScenePrimitiveItr = Ent->IteratePrimitives();
		while (ScenePrimitiveItr)
		{
			auto Primitive = (*ScenePrimitiveItr)->GetPrimitive();

			if(Ent->IsVisible())
				WorldScene->AddPrimitive(Primitive);
			else
				WorldScene->RemovePrimitive(Primitive);

			++ScenePrimitiveItr;
		}
	}
}

#include "btBulletDynamicsCommon.h"
#include "World.h"
#include "Scene.h"
#include "Physics.h"

namespace Ry
{

	World::World()
	{
		btDefaultCollisionConfiguration* CollisionConfig = new btDefaultCollisionConfiguration;

		btCollisionDispatcher* CollisionDispatcher = new btCollisionDispatcher(CollisionConfig);

		btBroadphaseInterface* Cache = new btDbvtBroadphase;

		btSequentialImpulseConstraintSolver* Solver = new btSequentialImpulseConstraintSolver;
		
		PhysicsWorld = new btDiscreteDynamicsWorld(CollisionDispatcher, Cache, Solver, CollisionConfig);
		PhysicsWorld->setGravity(btVector3(0, -10, 0));
	}

	World::~World()
	{

	}

	void World::AddEntity(Ry::SharedPtr<Entity> Ent)
	{
		Entities.Add(Ent);

		Ent->InitPhysics();

		// Iterate through collision bodies on entity and add them to the world
		Ry::ArrayList<CollisionComponent*>& CollisionComponents = Ent->GetCollisionComponents();
		for(CollisionComponent* Comp : CollisionComponents)
		{
			if(Comp->CollisionEnabled)
			{
				SharedPtr<RigidBody> PhysicsBody = Ent->GetBody();

				// Add physics body to world
				PhysicsWorld->addRigidBody(PhysicsBody->Body);

			}
		}

		if(Ent->PhysicsEnabled)
		{
			if(Ent->CollisionEnabled)
			{
				SharedPtr<RigidBody> PhysicsBody = Ent->GetBody();

				// Add physics body to world
				PhysicsWorld->addRigidBody(PhysicsBody->Body);
			}
		}
	}

	void World::RemoveEntity(Ry::SharedPtr<Entity> Ent)
	{
		Entities.Remove(Ent);
	}
	
	void World::Update(float Delta)
	{
		// Update the physics world
		PhysicsWorld->stepSimulation(Delta, 10);
		
		for (SharedPtr<Entity> Ent : Entities)
		{
			// Check if we need to update this entities position
			
			Ent->Update(Delta);
		}
	}

	void World::Render()
	{
		Ry::SceneLighting Lighting;
		Lighting.Light.Color = Ry::Color(0.8f, 0.6f, 0.6f, 1.0f);
		Lighting.Light.Intensity = 0.8f;
		Lighting.Light.Direction = Ry::Vector3(-1.0, -1.0, 0.0f);
		
		Ry::ObjectPipeline->Begin();
		for (SharedPtr<Entity> Ent : Entities)
		{
			// Check if there is a mesh component
			if (Ent->CanRender())
			{
				Ry::ArrayList<MeshComponent*>& MeshComponents = Ent->GetMeshComponents();

				for (MeshComponent* MeshComp : MeshComponents)
				{
					// Render mesh with model matrix and camera matrix
					ObjectPipeline->RenderMesh(MeshComp->GetRuntimeMesh(), MeshComp->GetWorldTransform().get_transform(), GlobalCamera->get_view_proj(), Lighting);
				}
			}
		}
		Ry::ObjectPipeline->End();
	}
	
}

#include "World2D.h"

namespace Ry
{

	World2D::World2D(SwapChain* SC)
	{
		WorldScene = new Scene2D(SC);
	}

	World2D::~World2D()
	{
	}

	void World2D::Resize(int32 Width, int32 Height)
	{
		WorldScene->Resize(Width, Height);
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

		OnEntityDirty(Ent.Get());
		
		Ent->ClearComponents();
		
		Entities.Remove(Ent);
	}

	void World2D::Update(float Delta)
	{
		for(SharedPtr<Entity2D> Ent : Entities)
		{
			Ent->Update(Delta);
		}

		WorldScene->Update(Delta);
	}

	void World2D::Draw()
	{
		// Draw the scene
		WorldScene->Render();
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

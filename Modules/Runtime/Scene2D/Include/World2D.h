#pragma once

#include "Data/ArrayList.h"
#include "Entity2D.h"

namespace Ry
{

	class SCENE2D_MODULE World2D
	{
	public:

		World2D(SwapChain* SC);
		~World2D();

		void Resize(int32 Width, int32 Height);

		void AddEntity(const Ry::SharedPtr<Entity2D> Ent);
		void RemoveEntity(const Ry::SharedPtr<Entity2D> Ent);

		void Update(float Delta);

		// Todo: separate from main thread
		void Draw();

	private:

		void OnEntityDirty(Entity2D* Ent);

		Ry::Scene2D* WorldScene;

		Ry::ArrayList < SharedPtr<Entity2D>> Entities;
		
	};

}

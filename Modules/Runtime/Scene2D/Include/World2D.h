#pragma once

#include "Data/ArrayList.h"
#include "Entity2D.h"
#include "Camera.h"

class b2World;
class b2ContactListener;
class b2Contact;

constexpr float PixelsToMeters(float Pixels) { return Pixels / 50.0f; }
constexpr float MetersToPixels(float Meters) { return Meters * 50.0f; }

namespace Ry
{
	struct TmxMap;
	class VectorFontAsset;

	class SCENE2D_MODULE CameraController2D
	{
	public:
		CameraController2D() = default;

		virtual void Update(float Delta, Ry::Camera2D& Camera) = 0;
	};

	class SCENE2D_MODULE World2D
	{
	public:

		World2D(SwapChain* SC, Ry::Vector2 Gravity = {});
		~World2D();

		void Resize(int32 Width, int32 Height);

		void SetCameraController(Ry::CameraController2D* Controller);

		void AddEntity(const Ry::SharedPtr<Entity2D> Ent);
		void RemoveEntity(const Ry::SharedPtr<Entity2D> Ent);

		void Update(float Delta);

		void UpdateStaticGeometry();

		// Todo: separate from main thread
		void Draw();

		b2World* GetPhysicsWorld() const;

	private:

		Ry::Camera2D* Camera;
		
		CameraController2D* CameraController;

		void OnBeginContact(b2Contact* Contact);
		void OnEndContact(b2Contact* Contact);

		b2ContactListener* ContactListener;
		b2World* PhysicsWorld;

		void OnEntityDirty(Entity2D* Ent);

		Ry::Scene2D* WorldScene;

		Ry::ArrayList < SharedPtr<Entity2D>> Entities;
		Ry::ArrayList < SharedPtr<Entity2D>> KillEnts;

	};

	SCENE2D_MODULE void CreateFromTmx(Ry::World2D* In, const TmxMap& Map, VectorFontAsset* FontAsset);


}

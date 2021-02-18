#pragma once

#include "Core/Core.h"
#include "TransformComponent.h"
#include "Camera.h"
#include "Scene.h"

namespace Ry
{

	enum class ProjectionMode
	{
		PERSPECTIVE, ORTHOGRAPHIC
	};

	class SCENE_MODULE CameraComponent : public TransformComponent
	{
	public:

		bool UseOnStart;

		CameraComponent()
		{
			this->UseOnStart = true;
		}

		virtual ~CameraComponent() = default;
		
		Ry::Camera* GetCamera()
		{
			return Camera.Get();
		}

		void InitComponent() override
		{
			if(UseOnStart)
			{
				GlobalCamera = Camera;
			}
		}

		/**
		 * Set the position of the camera
		 */
		void Update(float Delta) override
		{
			Component::Update(Delta);

			Transform WorldTransform = GetWorldTransform();
			Camera->transform.position = WorldTransform.position;
			Camera->transform.rotation = WorldTransform.rotation;
			Camera->transform.scale = WorldTransform.scale;
		}

		void InitPerspective(float FieldOfView, float Aspect, float ZNear, float ZFar)
		{
			Camera.Reset(new PerspectiveCamera(FieldOfView, Aspect, ZNear, ZFar));
		}

		void InitOrtho(float Left, float Right, float Bottom, float Top)
		{
			Camera.Reset(new OrthoCamera(Left, Right, Bottom, Top));
		}
		
	private:

		SharedPtr<Ry::Camera> Camera;
	};
	
}

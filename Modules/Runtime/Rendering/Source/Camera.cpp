#include "Camera.h"
#include "Core/Globals.h"
#include "RyMath.h"

namespace Ry
{
	
	Matrix4 Camera::GetProjection() const
	{
		return Projection;
	}

	Matrix4 Camera::GetViewProjection() const
	{
		return Projection * GetView();
	}

	Camera2D::Camera2D(float Width, float Height):
	Camera2D(-Width / 2, Width / 2, -Height / 2, Height / 2)
	{
	}

	Camera2D::Camera2D(float Left, float Right, float Bottom, float Top)
	{
	//	float aspect = Ry::GetViewportWidth() / (float)Ry::GetViewportHeight();

		// TODO: needs to account for aspect ratio!!!!
		Projection = ortho4(Left, Right, Bottom, Top, -1.0f, 1.0f);
	}

	Vector2 Camera2D::ScreenToWorld(Vector2 Screen) const
	{
		float hw = Ry::GetViewportWidth() / 2.0f;
		float hh = Ry::GetViewportHeight() / 2.0f;

		Matrix4 inverse_view = inverse(GetView());
		Matrix4 inverse_proj = inverse(GetProjection());

		Vector4 ndc = Vector4((Screen.x - hw) / hw, (hh - Screen.y) / hh, 0.0f, 1.0f);
		Vector4 world = inverse_view * (inverse_proj * ndc);

		return Vector2(world.x, world.y);
	}

	void Camera2D::Update()
	{
	}

	void Camera2D::Resize(int32 Width, int32 Height)
	{
		//Projection = ortho4(0.0f, Width, 0.0f, Height, 1.0f, -1.0f);

		//Projection = ortho4(-Width / 2.0f, static_cast<float>(Width / 2.0f), Height, 0, 1.0f, -1.0f);

		Projection = ortho4(-Width/2.0f, static_cast<float>(Width/2.0f), -Height/2.0f, static_cast<float>(Height/2.0f), 1.0f, -1.0f);
	}

	Matrix4 Camera2D::GetView() const
	{
		return inverse(Transform.AsMatrix4());
	}

	OrthoCamera::OrthoCamera(float width, float height):
		OrthoCamera(-width / 2, width / 2, -height / 2, height / 2)
	{
		
	}
	
	OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
	{
		//float aspect = Ry::GetViewportWidth() / (float) Ry::GetViewportHeight();
	
		// TODO: needs to account for aspect ratio!!!!
		Projection = ortho4(left, right, bottom, top, 1.0f, -1.0f);
	}
	
	Vector2 OrthoCamera::screen_to_world(Vector2 screen) const
	{
		float hw = Ry::GetViewportWidth() / 2.0f;
		float hh = Ry::GetViewportHeight() / 2.0f;
	
		Matrix4 inverse_view = inverse(GetView());
		Matrix4 inverse_proj = inverse(GetProjection());
	
		Vector4 ndc = Vector4((screen.x - hw) / hw, (hh - screen.y) / hh, 0.0f, 1.0f);
		Vector4 world = inverse_view * (inverse_proj * ndc);
	
		return Vector2(world.x, world.y);
	}

	void OrthoCamera::update()
	{
		//Projection = ortho4(aspect, fov, z_near, z_far);
	}

	void OrthoCamera::resize(int32 width, int32 height)
	{
		Projection = ortho4(0, static_cast<float>(width), 0, static_cast<float>(height), 1.0f, -1.0f);
	}

	PerspectiveCamera::PerspectiveCamera(float aspect, float fov, float z_near, float z_far)
	{
		this->aspect = aspect;
		this->fov = fov;
		this->z_near = z_near;
		this->z_far = z_far;

		Update();
	}

	void PerspectiveCamera::Resize(int32 width, int32 height)
	{
		aspect = ((float)width) / height;
		Update();
	}

	void PerspectiveCamera::Update()
	{
		Projection = perspective4(aspect, fov, z_near, z_far);
	}
	
}
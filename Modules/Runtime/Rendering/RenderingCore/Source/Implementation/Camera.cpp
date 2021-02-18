#include "Camera.h"
#include "Core/Globals.h"
#include "RyMath.h"

namespace Ry
{

	Matrix4 Camera::get_proj() const
	{
		return proj;
	}

	Matrix4 Camera::get_view() const
	{
		// TODO: make this more efficient, maybe use caching
		// Use opposite transform for camera effect (position of camera transforms everything by negative position vector)
		return transform.get_inverse_transform();
	}

	Matrix4 Camera::get_view_proj() const
	{
		return proj * get_view();
	}
	
	OrthoCamera::OrthoCamera(float width, float height):
		OrthoCamera(-width / 2, width / 2, -height / 2, height / 2)
	{
		
	}
	
	OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
	{
		float aspect = Ry::GetViewportWidth() / (float) Ry::GetViewportHeight();
	
		// TODO: needs to account for aspect ratio!!!!
		proj = ortho4(left, right, bottom, top, 1.0f, -1.0f);
	}
	
	Vector2 OrthoCamera::screen_to_world(Vector2 screen) const
	{
		float hw = Ry::GetViewportWidth() / 2.0f;
		float hh = Ry::GetViewportHeight() / 2.0f;
	
		Matrix4 inverse_view = inverse(get_view());
		Matrix4 inverse_proj = inverse(get_proj());
	
		Vector4 ndc = Vector4((screen.x - hw) / hw, (hh - screen.y) / hh, 0.0f, 1.0f);
		Vector4 world = inverse_view * (inverse_proj * ndc);
	
		return Vector2(world.x, world.y);
	}

	void OrthoCamera::update()
	{

	}

	void OrthoCamera::resize(int32 width, int32 height)
	{
		proj = ortho4(0, static_cast<float>(width), 0, static_cast<float>(height), 1.0f, -1.0f);
	}

	PerspectiveCamera::PerspectiveCamera(float aspect, float fov, float z_near, float z_far)
	{
		this->aspect = aspect;
		this->fov = fov;
		this->z_near = z_near;
		this->z_far = z_far;

		update();
	}

	void PerspectiveCamera::resize(int32 width, int32 height)
	{
		aspect = ((float)width) / height;
		update();
	}

	void PerspectiveCamera::update()
	{
		proj = perspective4(aspect, fov, z_near, z_far);
	}
	
}
#pragma once

#include "Core/Core.h"
#include "Transform.h"
#include "RenderingCoreGen.h"

namespace Ry
{

	/**
	 * An abstract camera that defines the viewing frustum and position of a camera in euclidean space.
	 */
	class RENDERINGCORE_MODULE Camera
	{

	public:

		/**
		 * The transform of the camera.
		 */
		Ry::Transform transform;

		/**
		 * @return Matrix4 The projection matrix of this camera.
		 */
		virtual Matrix4 get_proj() const;

		/**
		 * @return Matrix4 The view matrix of this camera.
		 */
		virtual Matrix4 get_view() const;

		/**
		 * @return Matrix4 The combined view and projection matrix.
		 */
		virtual Matrix4 get_view_proj() const;

		virtual void update() = 0;

		virtual void resize(int32 width, int32 height) = 0;

	protected:
		Matrix4 proj;
	};

	/**
	 * An orthographic camera that displays a 2D view.
	 */
	class RENDERINGCORE_MODULE OrthoCamera : public Camera
	{

	public:

		/**
		 * @param width The width of the viewing rectangle.
		 * @param height The height of the viewing rectangle.
		 */
		OrthoCamera(float width, float height);

		/**
		 * @param left The x coordinate that an object at the left of the screen would have
		 * @param right The x coordinate that an object at the right of the screen would have
		 * @param bottom The y coordinate that an object at the bottom of the screen would have
		 * Param top The y coordinate that an object at the top of the screen would have
		 */
		OrthoCamera(float left, float right, float bottom, float top);

		/**
		 * Converts a 2D screen position to a 2D world position.
		 * @param screen The screen pixel coordinates.
		 * @return Vector The world coordinates
		 */
		virtual Vector2 screen_to_world(Vector2 screen) const;

		virtual void update();

		virtual void resize(int32 width, int32 height);
	};

	/**
	 * A 3D perspective camera.
	 */
	class RENDERINGCORE_MODULE PerspectiveCamera : public Camera
	{

	public:

		float aspect;
		float fov;
		float z_near;
		float z_far;

		/**
		 * @param aspect The screen aspect ratio width / height
		 * @param fov The field of view of the viewing frustum
		 * @param z_near The z coordinate at which objects that are behind get clipped
		 * @param z_far The z coordinate at which objects that are ahead of get clipped
		 */
		PerspectiveCamera(float aspect, float fov, float z_near, float z_far);

		virtual void update();

		virtual void resize(int32 width, int32 height);
	};
}
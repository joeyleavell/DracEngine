#pragma once

#include "RyMath.h"

namespace Ry
{

	/**
	 * Translate, rotation, scale transform.
	 */
	class MATH_MODULE Transform
	{

	public:

		/**
		 * The translation of the transform.
		 */
		Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

		/**
		 * Each component defines the corresponding axis of rotation. X-Axis rotation is roll,
		 * Y-Axis rotation is pitch, and Z-Axis rotation is yaw.
		 */
		Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
		
		/**
		 * The scale of the transform.
		 */
		Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);

		Transform()
		{

		}

		Transform(float X, float Y, float Z, float Rx, float Ry, float Rz)
		{
			position = Ry::Vector3(X, Y, Z);
			rotation = Ry::Vector3(Rx, Ry, Rz);
		}

		Vector3 get_forward() const;

		Vector3 get_right() const;

		Vector3 get_up() const;

		Matrix4 get_rotation_mat() const;

		/**
		 * Looks at a point, using the transforms position as the current eye.
		 */
		void LookAt(const Ry::Vector3& Position);

		/**
		 * Places this transform radius distance away from position, given the elevation (pitch) and azimuth (heading) of the position.
		 */
		void Orbit(const Ry::Vector3& Position, float Elevation, float Azimuth, float Radius);

		/**
		 * Gets the matrix representation of this transform.
		 */
		Matrix4 get_transform() const;

		/**
		 * Gets the matrix representation of this transform in the opposite direction.
		 */
		Matrix4 get_inverse_transform() const;

		Transform operator*(const Transform& Right);

	};

	MATH_MODULE Transform LinearInterp(const Transform& A, const Transform& B, float Delta);

}
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

		Transform(float X, float Y, float Z, float Rx = 0.0f, float Ry = 0.0f, float Rz = 0.0f, float Scale=1.0f)
		{
			position = Ry::Vector3(X, Y, Z);
			rotation = Ry::Vector3(Rx, Ry, Rz);
			scale = Ry::Vector3(Scale, Scale, Scale);

		}

		Vector3 get_forward() const;

		Vector3 get_right() const;

		Vector3 get_up() const;

		Matrix4 get_rotation_mat() const;

		/**
		 * Looks at a point, using the transforms position as the current eye.
		 */
		void LookAt(const Ry::Vector3& Position, const Ry::Vector3& Eye);

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

	MATH_MODULE Vector3 CombineRot(const Vector3& A, const Vector3& B);


	MATH_MODULE Vector3 RotInterp(const Vector3& A, const Vector3& B, float Delta);
	MATH_MODULE Transform LinearInterp(const Transform& A, const Transform& B, float Delta);

}
#pragma once

#include "RyMath.h"

namespace Ry
{

	struct MATH_MODULE Transform2D
	{
		Ry::Vector2 Position{ 0.0f, 0.0f };
		Ry::Vector2 Scale{ 1.0f, 1.0f };
		float Rotation{ 0.0f };

		Ry::Vector2 GetForward()
		{
			float Sin = sin(DEG_TO_RAD(Rotation) + PI / 2.0f);
			float Cos = cos(DEG_TO_RAD(Rotation) + PI / 2.0f);
			
			Vector2 Forward(Cos, Sin);

			return normalized(Forward);
		}

		Ry::Vector2 GetRight()
		{
			float Sin = sin(DEG_TO_RAD(Rotation));
			float Cos = cos(DEG_TO_RAD(Rotation));

			Vector2 Right (Cos, Sin);

			return normalized(Right);
		}

		Ry::Matrix4 AsMatrix4() const
		{
			Matrix4 Res = id4();
			Ry::Matrix3 R = Ry::Rotation2DMatrix(Rotation);
			Ry::Matrix3 S = Ry::Scale2DMatrix(Scale.x, Scale.y);
			Matrix3 Mat3 = R * S;
			for (int32 R = 0; R < 2; R++)
				for (int32 C = 0; C < 2; C++)
					Res[R][C] = Mat3[R][C];
			Res[0][3] = Position.x;
			Res[1][3] = Position.y;

			return Res;
		}
		
		Ry::Matrix3 AsMatrix() const
		{
			Ry::Matrix3 T = Ry::Translation2DMatrix(Position.x, Position.y);
			Ry::Matrix3 R = Ry::Rotation2DMatrix(Rotation);
			Ry::Matrix3 S = Ry::Scale2DMatrix(Scale.x, Scale.y);

			return T * R * S;
		}

		Ry::Transform2D Compose(const Transform2D& Other)
		{
			Transform2D Res;
			Res.Position = Position + Other.Position;
			Res.Rotation = Rotation + Other.Rotation;
			Res.Scale.x = Scale.x * Other.Scale.x;
			Res.Scale.y = Scale.y * Other.Scale.y;

			return Res;
		}

		friend Matrix3 ComposeMatrix(const Matrix3& Left, const Transform2D& Transform);
	};

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
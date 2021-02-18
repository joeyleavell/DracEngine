#include "Transform.h"
#include "Glm/glm.hpp"
#include "Glm/gtc/quaternion.hpp"

namespace Ry
{

	Vector3 Transform::get_forward() const
	{
		Vector4 result = get_rotation_mat() * Vector4(0.0f, 0.0f, -1.0f, 1.0f);

		return normalized(Vector3(result.x, result.y, result.z));
	}

	Vector3 Transform::get_right() const
	{
		Vector4 result = get_rotation_mat() * Vector4(-1.0f, 0.0f, 0.0f, 1.0f);

		return normalized(Vector3(result.x, result.y, result.z));
	}

	Vector3 Transform::get_up() const
	{
		Vector4 result = get_rotation_mat() * Vector4(0.0f, 1.0f, 0.0f, 1.0f);

		return normalized(Vector3(result.x, result.y, result.z));
	}

	Matrix4 Transform::get_rotation_mat() const
	{
		Matrix4 rotation_mat_x = rotatex4(rotation.x);
		Matrix4 rotation_mat_y = rotatey4(rotation.y);
		Matrix4 rotation_mat_z = rotatez4(rotation.z);

		return rotation_mat_z * rotation_mat_y * rotation_mat_x;
	}

	void Transform::LookAt(const Ry::Vector3& Position)
	{
		
	}

	void Transform::Orbit(const Ry::Vector3& Position, float Elevation, float Azimuth, float Radius)
	{
		Ry::Vector3 UpVector = Ry::Vector3(0.0f, 1.0f, 0.0f);

		Ry::Vector3 Forward = Ry::Vector3(0.0f, 0.0f, -1.0f);
		Ry::Vector3 AzimuthVec = Forward.RotatedVector(UpVector, Azimuth);
		Ry::Vector3 AzimuthElevationVec = Forward.RotatedVector(UpVector, Azimuth);
		Ry::Vector3 Direction = normalized((AzimuthElevationVec * -1.0f));

		this->position = Position + Direction * Radius;
	}

	Matrix4 Transform::get_transform() const
	{
		Matrix4 translation_mat = translation4(position.x, position.y, position.z);
		Matrix4 rotation_mat_x = rotatex4(rotation.x);
		Matrix4 rotation_mat_y = rotatey4(rotation.y);
		Matrix4 rotation_mat_z = rotatez4(rotation.z);
		Matrix4 scale_mat = scale4(scale.x, scale.y, scale.z);

		return translation_mat * rotation_mat_z * rotation_mat_y * rotation_mat_x * scale_mat;
	}

	Matrix4 Transform::get_inverse_transform() const
	{
		Matrix4 translation_mat = translation4(position.x, position.y, position.z);
		Matrix4 scale_mat = scale4(scale.x, scale.y, scale.z);

		return inverse(translation_mat * rotatez4(rotation.z) * rotatey4(rotation.y) * rotatex4(rotation.x) * scale_mat);
	}

	Transform Transform::operator*(const Transform& Right)
	{
		// Compose the transform
		Transform Result;
		Result.position = position + Right.position;
		Result.scale = Vector3(scale.x * Right.scale.x, scale.y * Right.scale.y, scale.z * Right.scale.z);

		glm::quat ThisRotQuat = glm::quat(glm::vec3(DEG_TO_RAD(rotation.x), DEG_TO_RAD(rotation.y), DEG_TO_RAD(rotation.z)));
		glm::quat OtherRotQuat = glm::quat(glm::vec3(DEG_TO_RAD(Right.rotation.x), DEG_TO_RAD(Right.rotation.y), DEG_TO_RAD(Right.rotation.z)));
		glm::quat Product = OtherRotQuat * ThisRotQuat;
		glm::vec3 EulerAngles = glm::eulerAngles(Product);
		Result.rotation = Ry::Vector3(RAD_TO_DEG(EulerAngles.x), RAD_TO_DEG(EulerAngles.y), RAD_TO_DEG(EulerAngles.z));

		return Result;
	}

	Transform LinearInterp(const Transform& A, const Transform& B, float Delta)
	{
		// Extract parameters from matrix		
		Transform Result;
		Result.position = A.position * (1 - Delta) + B.position * Delta;
		Result.scale    = A.scale * (1 - Delta) + B.scale * Delta;

		glm::quat ThisRotQuat = glm::quat(glm::vec3(A.rotation.x, A.rotation.y, A.rotation.z));
		glm::quat OtherRotQuat = glm::quat(glm::vec3(B.rotation.x, B.rotation.y, B.rotation.z));
		glm::quat Mix = glm::mix(ThisRotQuat, OtherRotQuat, Delta);
		glm::vec3 EulerAngles = glm::eulerAngles(Mix);
		Result.rotation = Ry::Vector3(EulerAngles.x, EulerAngles.y, EulerAngles.z);

		return Result;
	}

}
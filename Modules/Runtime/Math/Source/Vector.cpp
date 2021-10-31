#include "Vector.h"
#include "RyMath.h"
#include "Core/String.h"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace Ry
{
	
	
	/************************************************************************/
	/* VECTOR CONSTRUCTORS                                                  */
	/************************************************************************/
	//
	// Vector2::Vector(float a) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[0]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[0]),
	// 	a(data[0])
	// {
	// 	data[0] = a;
	// 	data[1] = a;
	// }

	// Vector3::Vector(float a) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[0])
	// {
	// 	data[0] = a;
	// 	data[1] = a;
	// 	data[2] = a;
	// }
	//
	// Vector4::Vector(float a) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[3]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[3])
	// {
	// 	data[0] = a;
	// 	data[1] = a;
	// 	data[2] = a;
	// 	data[3] = a;
	// }

	// Vector2::Vector(float p_x, float p_y) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[0]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[0]),
	// 	a(data[0])
	// {
	// 	data[0] = p_x;
	// 	data[1] = p_y;
	// }

	// Vector3::Vector(float p_x, float p_y) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[3]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[3])
	// {
	// 	data[0] = p_x;
	// 	data[1] = p_y;
	// 	data[2] = 0.0f;
	// }
	//
	// Vector4::Vector(float p_x, float p_y) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[3]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[3])
	// {
	// 	data[0] = p_x;
	// 	data[1] = p_y;
	// 	data[2] = 0.0f;
	// 	data[3] = 0.0f;
	// }
	//
	// Vector3::Vector(float p_x, float p_y, float p_z) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[0])
	// {
	// 	data[0] = p_x;
	// 	data[1] = p_y;
	// 	data[2] = p_z;
	// }
	//
	// Vector4::Vector(float p_x, float p_y, float p_z) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[3]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[3])
	// {
	// 	data[0] = p_x;
	// 	data[1] = p_y;
	// 	data[2] = p_z;
	// 	data[3] = 0.0f;
	// }
	//
	// Vector4::Vector(float p_x, float p_y, float p_z, float p_w):
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[3]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[3])
	// {
	// 	data[0] = p_x;
	// 	data[1] = p_y;
	// 	data[2] = p_z;
	// 	data[3] = p_w;
	// }

	// Vector<2>::Vector(const Vector<2>& o) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[0]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[0]),
	// 	a(data[0])
	// {
	// 	x = o.x;
	// 	y = o.y;
	// }

	// Vector<3>::Vector(const Vector<3>& o) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[0])
	// {
	// 	x = o.x;
	// 	y = o.y;
	// 	z = o.z;
	// }
	//
	// Vector<4>::Vector(const Vector<4>& o) :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[3]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[3])
	// {
	// 	x = o.x;
	// 	y = o.y;
	// 	z = o.z;
	// 	w = o.w;
	// }

	// Vector<2>::Vector() :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[0]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[0]),
	// 	a(data[0])
	// {
	// 	data[0] = 0.0f;
	// 	data[1] = 0.0f;
	// }

	// Vector<3>::Vector() :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[0]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[0])
	// {
	// 	data[0] = 0.0f;
	// 	data[1] = 0.0f;
	// 	data[2] = 0.0f;
	// }
	//
	// Vector<4>::Vector() :
	// 	x(data[0]),
	// 	y(data[1]),
	// 	z(data[2]),
	// 	w(data[3]),
	// 	r(data[0]),
	// 	g(data[1]),
	// 	b(data[2]),
	// 	a(data[3])
	// {
	// 	data[0] = 0.0f;
	// 	data[1] = 0.0f;
	// 	data[2] = 0.0f;
	// 	data[3] = 0.0f;
	// }


	//
	// template <uint32 S>
	// Vector<S>::~Vector()
	// {
	//
	// }

	/************************************************************************/
	/* VECTOR OPERATIONS                                                                     */
	/************************************************************************/

	template <uint32 S>
	Vector<S>& Vector<S>::operator-=(const Vector<S>& o)
	{
		for (uint32 i = 0; i < S; i++)
		{
			data[i] -= o.data[i];
		}

		return *this;
	}

	template <uint32 S>
	Vector<S>& Vector<S>::operator+=(const Vector<S>& o)
	{
		for (uint32 i = 0; i < S; i++)
		{
			data[i] += o.data[i];
		}

		return *this;
	}

	template <uint32 S>
	Vector<S>& Vector<S>::operator*=(const Vector<S>& o)
	{
		for (uint32 i = 0; i < S; i++)
		{
			data[i] *= o.data[i];
		}

		return *this;
	}

	template <uint32 S>
	Vector<S>& Vector<S>::operator*=(float s)
	{
		for (uint32 i = 0; i < S; i++)
		{
			data[i] *= s;
		}

		return *this;
	}

	template <uint32 S>
	Vector<S> Vector<S>::operator-(const Vector<S>& o) const
	{
		Vector<S> result;

		for (uint32 i = 0; i < S; i++)
		{
			result.data[i] = data[i] - o.data[i];
		}

		return result;
	}

	template <uint32 S>
	Vector<S> Vector<S>::operator+(const Vector<S>& o) const
	{
		Vector<S> result;

		for (uint32 i = 0; i < S; i++)
		{
			result.data[i] = data[i] + o.data[i];
		}

		return result;
	}

	template <uint32 S>
	float Vector<S>::operator*(const Vector<S>& o) const
	{
		// Calculates the dot product.

		float result = 0.0f;

		for (uint32 i = 0; i < S; i++)
		{
			result += data[i] * o.data[i];
		}

		return result;
	}

	template <uint32 S>
	bool Vector<S>::operator==(const Vector<S>& o) const
	{
		for (uint32 i = 0; i < S; i++)
		{
			float ThisElement = data[i];
			float OtherElement = o.data[i];

			if(std::abs(ThisElement - OtherElement) > FP_TOLERANCE)
			{
				return false;
			}
		}

		return true;
	}

	template <uint32 S>
	int32 Vector<S>::DataSizeBytes() const
	{
		return sizeof(data);
	}

	template <uint32 S>
	bool Vector<S>::IsNearlyEqual(const Vector<S>& Other, float Threshold)
	{
		bool bEqual = true;
		for (int32 Val = 0; Val < S; Val++)
			if (!Ry::IsNearlyEqual(data[Val], Other.data[Val], Threshold))
				bEqual = false;

		return bEqual;
	}

	template <uint32 S>
	Vector<S> Vector<S>::operator*(float s) const
	{
		Vector<S> result;

		for (uint32 i = 0; i < S; i++)
		{
			result.data[i] = data[i] * s;
		}

		return result;
	}

	template <uint32 S>
	float& Vector<S>::operator[](uint32 index)
	{
		return data[index];
	}

	template <uint32 S>
	float Vector<S>::operator[](uint32 index) const
	{
		return data[index];
	}

	template <uint32 S>
	const float* Vector<S>::operator*() const
	{
		return data;
	}

	template <uint32 S>
	Vector<S>& Vector<S>::operator=(const Vector<S>& o)
	{
		for (uint32 i = 0; i < S; i++)
		{
			data[i] = o.data[i];
		}

		return *this;
	}

	template <uint32 S>
	String to_string(const Vector<S>& vec)
	{
		std::stringstream ss;
		ss << "[";

		for (uint32 i = 0; i < S; i++)
		{
			ss << std::setprecision(3) << vec.data[i];
			if (i < S - 1)
				ss << ", ";
		}

		ss << "]";

		return String(ss.str().c_str());
	}

	template <uint32 S>
	float magnitude(const Vector<S>& vec)
	{
		// Calculate
		float mag = 0.0f;
		for (uint32 i = 0; i < S; i++)
			mag += vec.data[i] * vec.data[i];

		return sqrt(mag);
	}

	template <uint32 S>
	Vector<S>& normalize(Vector<S>& vec)
	{
		float mag = magnitude(vec);

		for (uint32 i = 0; i < S; i++)
			vec[i] /= mag;

		return vec;
	}

	template <uint32 S>
	Vector<S> normalized(const Vector<S>& vec)
	{
		Vector<S> result;

		float mag = magnitude(vec);

		for (uint32 i = 0; i < S; i++)
			result[i] = vec[i] / mag;

		return result;
	}

	template <unsigned R_>
	float dot(const Vector<R_>& a, const Vector<R_>& b)
	{
		float result = 0.0f;

		for (uint32 i = 0; i < R_; i++)
		{
			result += a[i] * b[i];
		}

		return result;
	}


	Vector3 cross(const Vector3& a, const Vector3& b)
	{
		Vector3 result;

		result[0] = a.data[1] * b.data[2] - a.data[2] * b.data[1];
		result[1] = a.data[2] * b.data[0] - a.data[0] * b.data[2];
		result[2] = a.data[0] * b.data[1] - a.data[1] * b.data[0];

		return result;
	}

	Vector3 Vector3::RotateVector(const Vector3& Axis, float Theta)
	{
		*this = RotatedVector(Axis, Theta);

		return *this;
	}
	
	Vector3 Vector3::RotatedVector(const Vector3& Axis, float Theta) const
	{
		float Cos = cos(DEG_TO_RAD(Theta));
		float Sin = sin(DEG_TO_RAD(Theta));
		Vector3 kv = cross(Axis, *this);

		return *this * Cos + kv * Sin + (Axis * dot(Axis, *this)) * (1 - Cos);
	}

	Vector3 MakeRotation(const Vector3& Forward)
	{
		float pitch = 0.0f;
		//float roll = 0.0f;
		float yaw = 0.0f;

		yaw = angle(Vector3(Forward.x, 0.0f, Forward.z), Vector3(0.0f, 0.0f, 1.0f));
		pitch = -angle(Forward, Vector3(Forward.x, 0.0f, Forward.z));

// 		pitch = 0.0f;

		if (Forward.x < 0.0f)
		{
			yaw = -yaw;
		}

 		if (Forward.y < 0.0f)
 		{
 			pitch = -pitch;
 		}
		
		//std::cout << *to_string(forward) << std::endl;
		//std::cout << *to_string(Vector3(forward.x, 0.0f, forward.z)) << std::endl;
	//	std::cout << pitch << " " << roll << " " << yaw << std::endl;
	//	std::cout << "end of make rot" << std::endl;

		return Vector3(pitch, yaw, 0.0f);
	}

	float angle(const Vector3& a, const Vector3& b)
	{
		float in = dot(a, b) / (magnitude(a) * magnitude(b));
		
		if (in > 1.0f)
			in = 1.0f;
		else if (in < -1.0f)
			in = -1.0f;

		return RAD_TO_DEG(acos(in));
	}

	// Vectors
	template class MATH_MODULE Vector<4>;
	template class MATH_MODULE Vector<3>;
	template class MATH_MODULE Vector<2>;

	// Vector to string
	template MATH_MODULE String to_string(const Vector<2>& a);
	template MATH_MODULE String to_string(const Vector<3>& a);
	template MATH_MODULE String to_string(const Vector<4>& a);

	// Normalization
	template MATH_MODULE Vector<2> normalized(const Vector<2>& a);
	template MATH_MODULE Vector<3> normalized(const Vector<3>& a);
	template MATH_MODULE Vector<4> normalized(const Vector<4>& a);
	template MATH_MODULE Vector<2>& normalize(Vector<2>& a);
	template MATH_MODULE Vector<3>& normalize(Vector<3>& a);
	template MATH_MODULE Vector<4>& normalize(Vector<4>& a);

	// Magnitude
	template MATH_MODULE float magnitude(const Vector<2>& vec);
	template MATH_MODULE float magnitude(const Vector<3>& vec);
	template MATH_MODULE float magnitude(const Vector<4>& vec);


}
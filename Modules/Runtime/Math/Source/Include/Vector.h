#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "MathGen.h"

namespace Ry
{

	template <uint32 S>
	class MATH_MODULE Vector
	{

	public:

		Vector() = default;
		~Vector() = default;

		float* Pack(float* Out) const
		{
			for(int32 Element = 0; Element < S; Element++)
			{
				Out[Element] = data[Element];
			}

			return Out + S;
		}

		Vector& operator-=(const Vector& o);
		Vector& operator+=(const Vector& o);
		Vector& operator*=(const Vector& o);
		Vector& operator*=(float s);
		Vector operator-(const Vector& o) const;
		Vector operator+(const Vector& o) const;
		float operator*(const Vector& o) const;
		Vector operator*(float s) const;
		float& operator[](uint32 index);
		float operator[](uint32 index) const;

		Vector& operator=(const Vector& o);
		const float* operator*() const;

		bool operator==(const Vector& o) const;

		int32 DataSizeBytes() const;

		template <uint32 R_>
		friend String to_string(const Vector<R_>& vec);

		template <uint32 R_>
		friend float magnitude(const Vector<R_>& vec);

		template <uint32 R_>
		friend Vector<R_>& normalize(Vector<R_>& vec);

		template <uint32 R_>
		friend Vector<R_> normalized(const Vector<R_>& vec);

		template <uint32 R_>
		friend float dot(const Vector<R_>& a, const Vector<R_>& b);

	protected:

		float data[S];
	};


	class MATH_MODULE Vector2 : public Vector<2>
	{
	public:
		float& x;
		float& y;
		float& u;
		float& v;
		
		Vector2() : Vector2(0.0f, 0.0f) {};
		Vector2(float p_x) : Vector2(p_x, 0.0f) {};;
		Vector2(const Vector2& Other):Vector2(Other.x, Other.y){}
		Vector2(const Vector<2>& Other) :Vector2(Other[0], Other[1]) {}
		Vector2(float p_x, float p_y) :
		Vector(),
		x(data[0]),
		y(data[1]),
		u(data[0]),
		v(data[1])
		{
			data[0] = p_x;
			data[1] = p_y;
		};

		Vector2& operator=(const Vector2& o)
		{
			if (this == &o)
				return *this;
			
			x = o[0];
			y = o[1];

			return *this;
		};

	};

	class MATH_MODULE Vector3 : public Vector<3>
	{
	public:
		float& x;
		float& y;
		float& z;
		float& r;
		float& g;
		float& b;

		Vector3() : Vector3(0.0f, 0.0f, 0.0f) {};
		Vector3(float p_x) : Vector3(p_x, 0.0f, 0.0f) {};;
		Vector3(float p_x, float p_y) : Vector3(p_x, p_y, 0.0f) {};;
		Vector3(const Vector3& Other) : Vector3(Other.x, Other.y, Other.z) {}
		Vector3(const Vector<3>& Other) : Vector3(Other[0], Other[1], Other[2]) {}
		Vector3(float p_x, float p_y, float p_z) :
			Vector(),
			x(data[0]),
			y(data[1]),
			z(data[2]),
			r(data[0]),
			g(data[1]),
			b(data[2])
		{
			data[0] = p_x;
			data[1] = p_y;
			data[2] = p_z;
		};

		Vector3 RotateVector(const Vector3& Axis, float theta);
		Vector3 RotatedVector(const Vector3& Axis, float theta) const;

		MATH_MODULE friend Vector3 cross(const Vector3& a, const Vector3& b);
		MATH_MODULE friend float angle(const Vector3& a, const Vector3& b);

		Vector3& operator=(const Vector3& o)
		{
			if (this == &o)
				return *this;

			x = o[0];
			y = o[1];
			z = o[2];

			return *this;
		};

	};

	class MATH_MODULE Vector4 : public Vector<4>
	{
	public:
		float& x;
		float& y;
		float& z;
		float& w;
		float& r;
		float& g;
		float& b;
		float& a;

		Vector4() : Vector4(0.0f, 0.0f, 0.0f, 0.0f) {};
		Vector4(float p_x) : Vector4(p_x, 0.0f, 0.0f, 1.0f) {};;
		Vector4(float p_x, float p_y) : Vector4(p_x, p_y, 0.0f, 1.0f) {};;
		Vector4(float p_x, float p_y, float p_z) : Vector4(p_x, p_y, p_z, 1.0f) {};
		Vector4(const Vector<4>& Other) : Vector4(Other[0], Other[1], Other[2], Other[3]) {}
		Vector4(const Vector4& Other) :Vector4(Other.x, Other.y) {}
		Vector4(float p_x, float p_y, float p_z, float p_w) :
			Vector(),
			x(data[0]),
			y(data[1]),
			z(data[2]),
			w(data[3]),
			r(data[0]),
			g(data[1]),
			b(data[2]),
			a(data[3])
		{
			data[0] = p_x;
			data[1] = p_y;
			data[2] = p_z;
			data[3] = p_w;
		};

		Vector4& operator=(const Vector4& o)
		{
			if (this == &o)
				return *this;

			x = o[0];
			y = o[1];
			z = o[2];
			w = o[3];

			return *this;
		};
	};

	// For hlsl equivalence
	typedef Vector2 Float2;
	typedef Vector3 Float3;
	typedef Vector4 Float4;


	MATH_MODULE Vector3 MakeRotation(const Vector3& ForwardVector);
	

}
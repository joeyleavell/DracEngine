#pragma once

#include "Core/Core.h"
#include "Core/String.h"
#include "Vector.h"
#include "MathGen.h"

namespace Ry
{
	
	template <uint32 R, uint32 C>
	class MATH_MODULE Matrix
	{
	public:

		Matrix();
		Matrix(double a);
		Matrix(double a, double b, ...);
		~Matrix();

		// Matrix initial setup
		Matrix& id();
		Matrix& zero();
		Matrix& fill(double a);

		// Elementary operations
		Matrix& operator-=(const Matrix& o);
		Matrix& operator+=(const Matrix& o);
		Matrix& operator*=(double s);
		Matrix operator-(const Matrix& o) const;
		Matrix operator+(const Matrix& o) const;
		Matrix operator*(double s) const;
		float* operator[](uint32 r);
		const float* operator*() const;

		// Matrix multiplication has been implemented as a friend function since it takes three template arguments (inner dimensions agree)
		template<uint32 D, uint32 E, uint32 F>
		friend Matrix<D, F> operator*(const Matrix<D, E>& a, const Matrix<E, F>& b);

		template<uint32 R_, uint32 C_>
		friend Vector<R_> operator*(const Matrix<R_, C_>& mat, const Vector<C_>& v);

		template<uint32 R_, uint32 C_>
		friend Matrix<C_, R_> transpose(const Matrix<R_, C_>& a);

		// String representation
		template<uint32 R_, uint32 C_>
		friend String to_string(const Matrix<R_, C_>& m);

		// Determinants
		friend double deter(const Matrix<2, 2>& mat);
		friend double deter(const Matrix<3, 3>& mat);
		friend double deter(const Matrix<4, 4>& mat);

		// Inverses
		MATH_MODULE friend Matrix<2, 2> inverse(const Matrix<2, 2>& mat);
		MATH_MODULE friend Matrix<3, 3> inverse(const Matrix<3, 3>& mat);
		MATH_MODULE friend Matrix<4, 4> inverse(const Matrix<4, 4>& mat);

	private:

		float data[R * C];
	};

	// Matrix shorthands
	typedef Matrix<1, 1> Matrix1x1;
	typedef Matrix<1, 2> Matrix1x2;
	typedef Matrix<1, 3> Matrix1x3;
	typedef Matrix<1, 4> Matrix1x4;
	typedef Matrix<2, 1> Matrix2x1;
	typedef Matrix<2, 2> Matrix2x2;
	typedef Matrix<2, 3> Matrix2x3;
	typedef Matrix<2, 4> Matrix2x4;
	typedef Matrix<3, 1> Matrix3x1;
	typedef Matrix<3, 2> Matrix3x2;
	typedef Matrix<3, 3> Matrix3x3;
	typedef Matrix<3, 4> Matrix3x4;
	typedef Matrix<4, 1> Matrix4x1;
	typedef Matrix<4, 2> Matrix4x2;
	typedef Matrix<4, 3> Matrix4x3;
	typedef Matrix<4, 4> Matrix4x4;
	typedef Matrix1x1 Matrix1;
	typedef Matrix2x2 Matrix2;
	typedef Matrix3x3 Matrix3;
	typedef Matrix4x4 Matrix4;

	// For hlsl equivalence
	typedef Matrix<1, 1> Float1x1;
	typedef Matrix<1, 2> Float1x2;
	typedef Matrix<1, 3> Float1x3;
	typedef Matrix<1, 4> Float1x4;
	typedef Matrix<2, 1> Float2x1;
	typedef Matrix<2, 2> Float2x2;
	typedef Matrix<2, 3> Float2x3;
	typedef Matrix<2, 4> Float2x4;
	typedef Matrix<3, 1> Float3x1;
	typedef Matrix<3, 2> Float3x2;
	typedef Matrix<3, 3> Float3x3;
	typedef Matrix<3, 4> Float3x4;
	typedef Matrix<4, 1> Float4x1;
	typedef Matrix<4, 2> Float4x2;
	typedef Matrix<4, 3> Float4x3;
	typedef Matrix<4, 4> Float4x4;

	// Matrix utility functions
	MATH_MODULE Matrix1 id1();
	MATH_MODULE Matrix2 id2();
	MATH_MODULE Matrix3 id3();
	MATH_MODULE Matrix4 id4();

	// 2D Homogenous matrices
	MATH_MODULE Matrix3 Translation2DMatrix(double X, double Y);
	MATH_MODULE Matrix3 Rotation2DMatrix(double Rotation);
	MATH_MODULE Matrix3 Scale2DMatrix(double Sx, double Sy);

	// 4D transformation matrices
	MATH_MODULE Matrix4 translation4(float x, float y, float z);
	MATH_MODULE Matrix4 rotatex4(float rotation);
	MATH_MODULE Matrix4 rotatey4(float rotation);
	MATH_MODULE Matrix4 rotatez4(float rotation);
	MATH_MODULE Matrix4 scale4(float x, float y, float z);
	MATH_MODULE Matrix4 ortho4(float left, float right, float bottom, float top, float far, float near);
	MATH_MODULE Matrix3 ortho3(float left, float right, float bottom, float top, float far, float near);

	MATH_MODULE Matrix4 perspective4(float aspect, float fov, float z_near, float z_far, bool bFlipY = false);


	extern MATH_MODULE const Matrix4 IDENTITY_4;

}
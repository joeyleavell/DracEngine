#include "Matrix.h"
#include "Vector.h"
#include "RyMath.h"
#include <cstdarg>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace Ry
{

	template <uint32 R, uint32 C>
	Matrix<R, C>::Matrix()
	{
		for (uint32 i = 0; i < R * C; i++)
			data[i] = 0.0f;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C>::Matrix(float a)
	{
		for (uint32 i = 0; i < R * C; i++)
			data[i] = a;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C>::Matrix(float a, float b, ...)
	{
		data[0] = a;
		data[1] = b;

		va_list vargs;
		va_start(vargs, b);

		for (uint32 i = 2; i < R * C; i++)
		{
			data[i] = (float)va_arg(vargs, double);
		}

		va_end(vargs);

	}

	template <uint32 R, uint32 C>
	Matrix<R, C>::~Matrix()
	{

	}

	/************************************************************************/
	/* MATRIX CONSTRUCTION UTILITES                                         */
	/************************************************************************/

	template <uint32 R, uint32 C>
	Matrix<R, C>& Matrix<R, C>::id()
	{
		for (uint32 i = 0; i < R; i++)
		{
			for (uint32 j = 0; j < C; j++)
			{
				if (i == j)
					(*this)[i][j] = 1.0f;
				else
					(*this)[i][j] = 0.0f;
			}
		}

		return *this;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C>& Matrix<R, C>::zero()
	{
		for (uint32 i = 0; i < R * C; i++)
			data[i] = 0.0f;

		return *this;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C>& Matrix<R, C>::fill(float a)
	{
		for (uint32 i = 0; i < R * C; i++)
			data[i] = a;

		return *this;
	}

	/************************************************************************/
	/* MATRIX OPERATIONS                                                                     */
	/************************************************************************/

	template <uint32 R, uint32 C>
	Matrix<R, C>& Matrix<R, C>::operator-=(const Matrix& o)
	{
		for (uint32 i = 0; i < R * C; i++)
			data[i] -= o.data[i];

		return *this;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C>& Matrix<R, C>::operator+=(const Matrix& o)
	{
		for (uint32 i = 0; i < R * C; i++)
			data[i] += o.data[i];

		return *this;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C>& Matrix<R, C>::operator*=(float s)
	{
		for (uint32 i = 0; i < R * C; i++)
			data[i] *= s;

		return *this;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C> Matrix<R, C>::operator-(const Matrix& o) const
	{
		Matrix<R, C> result;

		for (uint32 i = 0; i < R * C; i++)
			result.data[i] = data[i] - o.data[i];

		return result;

	}

	template <uint32 R, uint32 C>
	Matrix<R, C> Matrix<R, C>::operator+(const Matrix& o) const
	{
		Matrix<R, C> result;

		for (uint32 i = 0; i < R * C; i++)
			result.data[i] = data[i] + o.data[i];

		return result;
	}

	template <uint32 R, uint32 C>
	Matrix<R, C> Matrix<R, C>::operator*(float s) const
	{
		Matrix<R, C> result;

		for (uint32 i = 0; i < R * C; i++)
			result.data[i] = data[i] * s;

		return result;
	}


	template <uint32 R, uint32 C>
	float* Matrix<R, C>::operator[](uint32 r)
	{
		return data + (r * C);
	}

	template <uint32 R, uint32 C>
	const float* Matrix<R, C>::operator*() const
	{
		return data;
	}

	template<uint32 D, uint32 E, uint32 F>
	Matrix<D, F> operator*(const Matrix<D, E>& a, const Matrix<E, F>& b)
	{
		Matrix<D, F> result;

		for (uint32 r = 0; r < D; r++)
		{
			for (int32 c = 0; c < F; c++)
			{
				float dot = 0.0f;

				for (uint32 i = 0; i < E; i++)
				{
					dot += a.data[r * D + i] * b.data[i * E + c];
				}

				result[r][c] = dot;
			}
		}

		return result;
	}

	template<uint32 R, uint32 C>
	Matrix<C, R> transpose(const Matrix<R, C>& a)
	{
		Matrix<C, R> result;

		for (uint32 r = 0; r < R; r++)
		{
			for (uint32 c = 0; c < C; c++)
			{
				result[c][r] = a.data[r * R + c];
			}
		}

		return result;
	}

	template <uint32 R, uint32 C>
	String to_string(const Matrix<R, C>& m)
	{
		std::stringstream ss;

		ss << std::setprecision(8) << std::fixed;
		for (uint32 r = 0; r < R; r++)
		{
			for (uint32 c = 0; c < C; c++)
			{
				ss << m.data[r * C + c] << " ";
			}

			ss << std::endl;
		}

		return String(ss.str().c_str());
	}

	// Determinant functions
	float deter(const Matrix<2, 2>& mat)
	{
		return mat.data[0 * 2 + 0] * mat.data[1 * 2 + 1] - mat.data[0 * 2 + 1] * mat.data[1 * 2 + 0];
	}

	float deter(const Matrix<3, 3>& mat)
	{
		float a = mat.data[0 * 3 + 0];
		float b = mat.data[0 * 3 + 1];
		float c = mat.data[0 * 3 + 2];

		float i = a * (mat.data[1 * 3 + 1] * mat.data[2 * 3 + 2] - mat.data[1 * 3 + 2] * mat.data[2 * 3 + 1]);
		float j = b * (mat.data[1 * 3 + 0] * mat.data[2 * 3 + 2] - mat.data[1 * 3 + 2] * mat.data[2 * 3 + 0]);
		float k = c * (mat.data[1 * 3 + 0] * mat.data[2 * 3 + 1] - mat.data[1 * 3 + 1] * mat.data[2 * 3 + 1]);

		return i - j + k;
	}

	float deter(const Matrix<4, 4>& mat)
	{
		const float* b = mat.data;
		float value =
			b[3] * b[6] * b[9] * b[12] - b[2] * b[7] * b[9] * b[12] - b[3] * b[5] * b[10] * b[12] + b[1] * b[7] * b[10] * b[12] +
			b[2] * b[5] * b[11] * b[12] - b[1] * b[6] * b[11] * b[12] - b[3] * b[6] * b[8] * b[13] + b[2] * b[7] * b[8] * b[13] +
			b[3] * b[4] * b[10] * b[13] - b[0] * b[7] * b[10] * b[13] - b[2] * b[4] * b[11] * b[13] + b[0] * b[6] * b[11] * b[13] +
			b[3] * b[5] * b[8] * b[14] - b[1] * b[7] * b[8] * b[14] - b[3] * b[4] * b[9] * b[14] + b[0] * b[7] * b[9] * b[14] +
			b[1] * b[4] * b[11] * b[14] - b[0] * b[5] * b[11] * b[14] - b[2] * b[5] * b[8] * b[15] + b[1] * b[6] * b[8] * b[15] +
			b[2] * b[4] * b[9] * b[15] - b[0] * b[6] * b[9] * b[15] - b[1] * b[4] * b[10] * b[15] + b[0] * b[5] * b[10] * b[15];
		return value;
	}

	// Inverse functions
	Matrix<2, 2> inverse(const Matrix<2, 2>& mat)
	{
		Matrix<2, 2> result;
		float d = deter(mat);

		return result;
	}

	Matrix<3, 3> inverse(const Matrix<3, 3>& mat)
	{

		// TODO: MAKE THIS FASTER LIKE THE CURRENT 4D inverse!

		float d = deter(mat);

		Matrix<3, 3> minors;

		for (uint32 r = 0; r < 3; r++)
		{
			for (uint32 c = 0; c < 3; c++)
			{
				Matrix<2, 2> minor;
				uint32 element = 0;
				for (uint32 rr = 0; rr < 3; rr++)
				{
					for (uint32 cc = 0; cc < 3; cc++)
					{
						if (rr != r && cc != c)
						{
							minor.data[element] = mat.data[rr * 3 + cc];
							element++;
						}
					}
				}

				minors[r][c] = deter(minor);
			}
		}

		// Make minors matrix into cofactors matrix
		minors[0][1] = -minors[0][1];
		minors[1][0] = -minors[1][0];
		minors[2][1] = -minors[2][1];
		minors[1][2] = -minors[1][2];

		// Transpose the minors matrix
		Matrix<3, 3> t = transpose(minors);

		// Divide each element by the determinant
		t *= 1.0f / d;

		return t;
	}

	Matrix<4, 4> inverse(const Matrix<4, 4>& mat)
	{
		Matrix<4, 4> result;

		float* a = result.data;
		const float* b = mat.data;

		a[0] = b[6] * b[11] * b[13] - b[7] * b[10] * b[13] + b[7] * b[9] * b[14] - b[5] * b[11] * b[14] - b[6] * b[9] * b[15] + b[5] * b[10] * b[15];
		a[1] = b[3] * b[10] * b[13] - b[2] * b[11] * b[13] - b[3] * b[9] * b[14] + b[1] * b[11] * b[14] + b[2] * b[9] * b[15] - b[1] * b[10] * b[15];
		a[2] = b[2] * b[7] * b[13] - b[3] * b[6] * b[13] + b[3] * b[5] * b[14] - b[1] * b[7] * b[14] - b[2] * b[5] * b[15] + b[1] * b[6] * b[15];
		a[3] = b[3] * b[6] * b[9] - b[2] * b[7] * b[9] - b[3] * b[5] * b[10] + b[1] * b[7] * b[10] + b[2] * b[5] * b[11] - b[1] * b[6] * b[11];
		a[4] = b[7] * b[10] * b[12] - b[6] * b[11] * b[12] - b[7] * b[8] * b[14] + b[4] * b[11] * b[14] + b[6] * b[8] * b[15] - b[4] * b[10] * b[15];
		a[5] = b[2] * b[11] * b[12] - b[3] * b[10] * b[12] + b[3] * b[8] * b[14] - b[0] * b[11] * b[14] - b[2] * b[8] * b[15] + b[0] * b[10] * b[15];
		a[6] = b[3] * b[6] * b[12] - b[2] * b[7] * b[12] - b[3] * b[4] * b[14] + b[0] * b[7] * b[14] + b[2] * b[4] * b[15] - b[0] * b[6] * b[15];
		a[7] = b[2] * b[7] * b[8] - b[3] * b[6] * b[8] + b[3] * b[4] * b[10] - b[0] * b[7] * b[10] - b[2] * b[4] * b[11] + b[0] * b[6] * b[11];
		a[8] = b[5] * b[11] * b[12] - b[7] * b[9] * b[12] + b[7] * b[8] * b[13] - b[4] * b[11] * b[13] - b[5] * b[8] * b[15] + b[4] * b[9] * b[15];
		a[9] = b[3] * b[9] * b[12] - b[1] * b[11] * b[12] - b[3] * b[8] * b[13] + b[0] * b[11] * b[13] + b[1] * b[8] * b[15] - b[0] * b[9] * b[15];
		a[10] = b[1] * b[7] * b[12] - b[3] * b[5] * b[12] + b[3] * b[4] * b[13] - b[0] * b[7] * b[13] - b[1] * b[4] * b[15] + b[0] * b[5] * b[15];
		a[11] = b[3] * b[5] * b[8] - b[1] * b[7] * b[8] - b[3] * b[4] * b[9] + b[0] * b[7] * b[9] + b[1] * b[4] * b[11] - b[0] * b[5] * b[11];
		a[12] = b[6] * b[9] * b[12] - b[5] * b[10] * b[12] - b[6] * b[8] * b[13] + b[4] * b[10] * b[13] + b[5] * b[8] * b[14] - b[4] * b[9] * b[14];
		a[13] = b[1] * b[10] * b[12] - b[2] * b[9] * b[12] + b[2] * b[8] * b[13] - b[0] * b[10] * b[13] - b[1] * b[8] * b[14] + b[0] * b[9] * b[14];
		a[14] = b[2] * b[5] * b[12] - b[1] * b[6] * b[12] - b[2] * b[4] * b[13] + b[0] * b[6] * b[13] + b[1] * b[4] * b[14] - b[0] * b[5] * b[14];
		a[15] = b[1] * b[6] * b[8] - b[2] * b[5] * b[8] + b[2] * b[4] * b[9] - b[0] * b[6] * b[9] - b[1] * b[4] * b[10] + b[0] * b[5] * b[10];

		result *= 1.0f / deter(result);

		return result;
	}

	Matrix<1, 1> id1()
	{
		Matrix<1, 1> m;
		m.id();

		return m;
	}

	Matrix<2, 2> id2()
	{
		Matrix<2, 2> m;
		m.id();

		return m;
	}

	Matrix<3, 3> id3()
	{
		Matrix<3, 3> m;
		m.id();

		return m;
	}

	Matrix<4, 4> id4()
	{
		Matrix<4, 4> m;
		m.id();

		return m;
	}

	/************************************************************************/
	/* TRANSFORMATION MATRICES                                                                     */
	/************************************************************************/

	Matrix<4, 4> translation4(float x, float y, float z)
	{
		Matrix<4, 4> result = id4();

		result[0][3] = x;
		result[1][3] = y;
		result[2][3] = z;

		return result;
	}

	Matrix<4, 4> rotatex4(float rotation)
	{
		Matrix<4, 4> m = id4();

		float cos = std::cos(DEG_TO_RAD(rotation));
		float sin = std::sin(DEG_TO_RAD(rotation));

		m[0][0] = 1.0f;
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;
		m[0][3] = 0.0f;

		m[1][0] = 0.0f;
		m[1][1] = cos;
		m[1][2] = -sin;
		m[1][3] = 0.0f;

		m[2][0] = 0.0f;
		m[2][1] = sin;
		m[2][2] = cos;
		m[2][3] = 0.0f;

		m[3][0] = 0.0f;
		m[3][1] = 0.0f;
		m[3][2] = 0.0f;
		m[3][3] = 1.0f;

		return m;
	}

	Matrix<4, 4> rotatey4(float rotation)
	{
		Matrix<4, 4> m = id4();

		float cos = std::cos(DEG_TO_RAD(rotation));
		float sin = std::sin(DEG_TO_RAD(rotation));

		m[0][0] = cos;
		m[0][1] = 0.0f;
		m[0][2] = sin;
		m[0][3] = 0.0f;

		m[1][0] = 0.0f;
		m[1][1] = 1.0f;
		m[1][2] = 0.0f;
		m[1][3] = 0.0f;

		m[2][0] = -sin;
		m[2][1] = 0.0f;
		m[2][2] = cos;
		m[2][3] = 0.0f;

		m[3][0] = 0.0f;
		m[3][1] = 0.0f;
		m[3][2] = 0.0f;
		m[3][3] = 1.0f;

		return m;
	}

	Matrix<4, 4> rotatez4(float rotation)
	{
		Matrix<4, 4> m = id4();

		float cos = std::cos(DEG_TO_RAD(rotation));
		float sin = std::sin(DEG_TO_RAD(rotation));

		m[0][0] = cos;
		m[0][1] = -sin;
		m[0][2] = 0.0f;
		m[0][3] = 0.0f;

		m[1][0] = sin;
		m[1][1] = cos;
		m[1][2] = 0.0f;
		m[1][3] = 0.0f;

		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = 1.0f;
		m[2][3] = 0.0f;

		m[3][0] = 0.0f;
		m[3][1] = 0.0f;
		m[3][2] = 0.0f;
		m[3][3] = 1.0f;

		return m;
	}

	Matrix<4, 4> scale4(float x, float y, float z)
	{
		Matrix<4, 4> result = id4();

		result[0][0] = x;
		result[1][1] = y;
		result[2][2] = z;

		return result;
	}

	Matrix<4, 4> ortho4(float left, float right, float bottom, float top, float far, float near)
	{
		Matrix<4, 4> m;

		m[0][0] = 2.0f / (right - left);
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;
		m[0][3] = -(right + left) / (right - left);

		m[1][0] = 0.0f;
		m[1][1] = 2.0f / (top - bottom);
		m[1][2] = 0.0f;
		m[1][3] = -(top + bottom) / (top - bottom);

		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = -2.0f / (far - near);
		m[2][3] = -(far + near) / (far - near);

		m[3][0] = 0.0f;
		m[3][1] = 0.0f;
		m[3][2] = 0.0f;
		m[3][3] = 1.0f;

		return m;
	}

	Matrix4 perspective4(float aspect, float fov, float z_near, float z_far, bool bFlipY)
	{
		float tan_half_fov = tan(DEG_TO_RAD(fov / 2.0f));

		Matrix<4, 4> m;

		m[0][0] = 1.0f / (aspect * tan_half_fov);
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;
		m[0][3] = 0.0f;

		m[1][0] = 0.0f;
		m[1][1] = 1.0f / (tan_half_fov) * (bFlipY ? -1.0f : 1.0f);
		m[1][2] = 0.0f;
		m[1][3] = 0.0f;

		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = -(z_far + z_near) / (z_far - z_near);
		m[2][3] = -(2 * z_far * z_near) / (z_far - z_near);

		m[3][0] = 0.0f;
		m[3][1] = 0.0f;
		m[3][2] = -1.0f;
		m[3][3] = 0.0f;

		return m;
	}

	template <uint32 R, uint32 C>
	Vector<R> operator*(const Matrix<R, C>& mat, const Vector<C>& v)
	{
		Vector<R> result;

		const float* m = mat.data;

		result[0] = m[0 * 4 + 0] * v[0] + m[0 * 4 + 1] * v[1] + m[0 * 4 + 2] * v[2] + m[0 * 4 + 3] * v[3];
		result[1] = m[1 * 4 + 0] * v[0] + m[1 * 4 + 1] * v[1] + m[1 * 4 + 2] * v[2] + m[1 * 4 + 3] * v[3];
		result[2] = m[2 * 4 + 0] * v[0] + m[2 * 4 + 1] * v[1] + m[2 * 4 + 2] * v[2] + m[2 * 4 + 3] * v[3];
		result[3] = m[3 * 4 + 0] * v[0] + m[3 * 4 + 1] * v[1] + m[3 * 4 + 2] * v[2] + m[3 * 4 + 3] * v[3];

		return result;
	}


	template MATH_MODULE class Matrix<1, 1>;
	template MATH_MODULE class Matrix<1, 2>;
	template MATH_MODULE class Matrix<1, 3>;
	template MATH_MODULE class Matrix<1, 4>;
	template MATH_MODULE class Matrix<2, 1>;
	template MATH_MODULE class Matrix<2, 2>;
	template MATH_MODULE class Matrix<2, 3>;
	template MATH_MODULE class Matrix<2, 4>;
	template MATH_MODULE class Matrix<3, 1>;
	template MATH_MODULE class Matrix<3, 2>;
	template MATH_MODULE class Matrix<3, 3>;
	template MATH_MODULE class Matrix<3, 4>;
	template MATH_MODULE class Matrix<4, 1>;
	template MATH_MODULE class Matrix<4, 2>;
	template MATH_MODULE class Matrix<4, 3>;
	template MATH_MODULE class Matrix<4, 4>;

	// Matrix-Matrix multiplication
	template MATH_MODULE Matrix4 operator*(const Matrix4& a, const Matrix4& b);
	template MATH_MODULE Matrix3 operator*(const Matrix3& a, const Matrix3& b);
	template MATH_MODULE Matrix2 operator*(const Matrix2& a, const Matrix2& b);
	template MATH_MODULE Matrix1 operator*(const Matrix1& a, const Matrix1& b);

	// Matrix-Vector multiplication
	template MATH_MODULE Vector<4> operator*(const Matrix4& a, const Vector<4>& b);
	template MATH_MODULE Vector<3> operator*(const Matrix3& a, const Vector<3>& b);
	template MATH_MODULE Vector<2> operator*(const Matrix2& a, const Vector<2>& b);


	// Matrix to string
	template MATH_MODULE String to_string(const Matrix4& a);
	template MATH_MODULE String to_string(const Matrix3& a);
	template MATH_MODULE String to_string(const Matrix2& a);

	const Matrix4 IDENTITY_4 = id4();

}
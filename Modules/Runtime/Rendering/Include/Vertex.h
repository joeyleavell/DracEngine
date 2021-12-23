#pragma once

#include "Core/Core.h"
#include "RyMath.h"
#include "RenderingGen.h"
#include "Algorithm/Algorithm.h"

namespace Ry
{

	/**
	 * Simple vertex storing only position.
	 */
	struct RENDERING_MODULE Vertex
	{
		virtual void Pack(float* data) const = 0;
	};

	struct RENDERING_MODULE Vertex_P_UV_N_T_Bi : public Vertex
	{
		Vector3 Position;
		Vector2 UV;
		Vector3 Normal;
		Vector3 Tangent;
		Vector3 BiTangent;

		bool operator==(const Vertex_P_UV_N_T_Bi& Other) const
		{
			return
				Position == Other.Position && 
				UV == Other.UV && 
				Normal == Other.Normal &&
				Tangent == Other.Tangent &&
				BiTangent == Other.BiTangent;
		}

		virtual void Pack(float* Out) const
		{
			// todo: optimize?
			BiTangent.Pack(
			Tangent.Pack(
			Normal.Pack(
			UV.Pack(
			Position.Pack(Out)))));
		}
	};

	struct RENDERING_MODULE Vertex1P : public Vertex
	{
		Vector3 Position;

		Vertex1P():
			Vertex1P(0.0f, 0.0f, 0.0f)
		{

		}

		Vertex1P(float x, float y, float z)
		{
			Position.x = x;
			Position.y = y;
			Position.z = z;
		}

		virtual void Pack(float* Out) const
		{
			Position.Pack(Out);
		}
	};

	/**
	 * Vertex storing position and color attributes.
	 */
	struct RENDERING_MODULE Vertex1P1C : public Vertex1P
	{
		Vector4 Color;

		Vertex1P1C(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float R = 0.0f, float G = 0.0f, float B = 0.0f, float A = 1.0f) :
			Vertex1P(X, Y, Z),
			Color(R, G, B, A){}

		virtual void Pack(float* Out) const
		{
			Color.Pack(Position.Pack(Out));
		}
	};

	/**
	 * Vertex storing position and texture coordinate attributes.
	 */
	struct RENDERING_MODULE Vertex1P1UV : public Vertex1P
	{
		Vector2 UV;

		Vertex1P1UV(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float U = 0.0f, float V = 0.0f) :
			Vertex1P(X, Y, Z),
			UV(U, V)
		{}

		virtual void Pack(float* Out) const
		{
			UV.Pack(Position.Pack(Out));
		}
	};

	/**
	 * Vertex storing position, texture coordinate, and normal attributes. This is probably the most common vertex storage container.
	 */
	struct RENDERING_MODULE Vertex1P1UV1N : public Vertex1P1UV
	{
		Vector3 Normal;

		Vertex1P1UV1N(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float U = 0.0f, float V = 0.0f, float Nx = 0.0f, float Ny = 0.0f, float Nz = 0.0f) :
			Vertex1P1UV(X, Y, Z, U, V),
			Normal(Nx, Ny, Nz)
		{}

		bool operator==(const Vertex1P1UV1N& Other) const;

		virtual void Pack(float* Out) const
		{
			Normal.Pack(UV.Pack(Position.Pack(Out)));
		}
	};

	/**
	 * Vertex storing position, texture coordinate, and color attributes.
	 */
	struct RENDERING_MODULE Vertex1P1UV1C : public Vertex1P1UV
	{
		Vector4 Color;

		Vertex1P1UV1C(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float U = 0.0f, float V = 0.0f, float R = 0.0f, float G = 0.0f, float B = 0.0f, float A = 0.0f) :
			Vertex1P1UV(X, Y, Z, U, V),
		Color(R, G, B, A)
		{}

		virtual void Pack(float* Out) const
		{
			Color.Pack(UV.Pack(Position.Pack(Out)));
		}
	};

	struct RENDERING_MODULE Vertex1P1UV1N1C : public Vertex1P1UV1N
	{
		Vector4 Color;

		Vertex1P1UV1N1C(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float U = 0.0f, float V = 0.0f, float Nx = 0.0f, float Ny = 0.0f, float Nz = 0.0f, float R = 0.0f, float G = 0.0f, float B = 0.0f, float A = 0.0f) :
			Vertex1P1UV1N(X, Y, Z, U, V, Nx, Ny, Nz),
			Color(R, G, B, A){}

		bool operator==(const Vertex1P1UV1N1C& Other) const;

		virtual void Pack(float* Out) const
		{
			Color.Pack(Normal.Pack(UV.Pack(Position.Pack(Out))));
		}
	};

	template <>
	struct EXPORT_ONLY Hash<Vertex1P1UV1N>
	{
		uint32 operator()(const Ry::Vertex1P1UV1N& Vert) const
		{
			float Data[30];
			Vert.Pack(Data);

			uint32 Seed = 31;

			for (int32 El = 0; El < 8; El++)
			{
				Seed ^= std::hash<float>{}(Data[El]) + 0x9e3779b9;// +(Seed << 6) + (Seed >> 2);
			}

			return Seed;
		}
	};

}
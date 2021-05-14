#pragma once

#include "Core/Core.h"
#include "Core/String.h"
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

	/**
	 * Defines meta information about a vertex attribute. A vertex attribute is an atomic element of a vertex.
	 */
	struct RENDERING_MODULE VertexAttrib
	{
		/**
		 * The name of the vertex attribute.
		 */
		String Name;

		/**
		 * The amount of elements in the vertex attribute, usually floats.
		 */
		int32 Size;

		VertexAttrib()
		{
			Name = "";
			Size = 0;
		}

		VertexAttrib(const String& n, int32 si)
		{
			Name = n;
			Size = si;
		}

		bool operator==(const VertexAttrib& other);
	};

	extern RENDERING_MODULE VertexAttrib pos;
	extern RENDERING_MODULE VertexAttrib color;
	extern RENDERING_MODULE VertexAttrib uv;
	extern RENDERING_MODULE VertexAttrib normal;
	extern RENDERING_MODULE VertexAttrib tangent;
	extern RENDERING_MODULE VertexAttrib bitangent;

	class RENDERING_MODULE VertexFormat
	{
	public:

		VertexFormat()
		{
		}

		VertexFormat(Ry::ArrayList<VertexAttrib> Attribs) :
		ElementCount(0),
		Attributes(Attribs)
		{
			CalculateParams();
		}

		VertexFormat(const VertexFormat& Ref)
		{
			this->Attributes = Ref.Attributes;
			this->ElementCount = Ref.ElementCount;
			this->PosOffset = Ref.PosOffset;
			this->UVOffset = Ref.UVOffset;
			this->NormalOffset = Ref.NormalOffset;
			this->TangentOffset = Ref.TangentOffset;
			this->BiTangentOffset = Ref.BiTangentOffset;
			this->ColorOffset = Ref.ColorOffset;
		}

		VertexFormat(VertexFormat&& Forward) noexcept
		{
			this->Attributes = Forward.Attributes;
			this->ElementCount = Forward.ElementCount;
			this->PosOffset = Forward.PosOffset;
			this->UVOffset = Forward.UVOffset;
			this->NormalOffset = Forward.NormalOffset;
			this->TangentOffset = Forward.TangentOffset;
			this->BiTangentOffset = Forward.BiTangentOffset;
			this->ColorOffset = Forward.ColorOffset;
		}

		int32 GetElementCount()
		{
			return ElementCount;
		}

		int32 GetPosOffset()
		{
			return PosOffset;
		}

		int32 GetColorOffset()
		{
			return ColorOffset;
		}

		int32 GetNormalOffset()
		{
			return NormalOffset;
		}

		int32 GetUVOffset()
		{
			return UVOffset;
		}

		int32 GetTangentOffset()
		{
			return TangentOffset;
		}

		int32 GetBiTangentOffset()
		{
			return BiTangentOffset;
		}

		int32 NumAttributes() const
		{
			return Attributes.GetSize();
		}

		const VertexAttrib& GetAttrib(int32 Index) const
		{
			return Attributes[Index];
		}

		void AddAttribute(const VertexAttrib& Attrib)
		{
			Attributes.Add(Attrib);
			CalculateParams();
		}

		void RemoveAttribute(const VertexAttrib& Attrib)
		{
			Attributes.Remove(Attrib);
			CalculateParams();
		}

		VertexFormat& operator=(const VertexFormat& Ref)
		{
			if (this == &Ref)
				return *this;
			
			this->Attributes = Ref.Attributes;
			this->ElementCount = Ref.ElementCount;
			this->PosOffset = Ref.PosOffset;
			this->UVOffset = Ref.UVOffset;
			this->NormalOffset = Ref.NormalOffset;
			this->TangentOffset = Ref.TangentOffset;
			this->BiTangentOffset = Ref.BiTangentOffset;
			this->ColorOffset = Ref.ColorOffset;

			return *this;
		}

	private:

		int32 ElementCount = 0;
		int32 PosOffset = -1;
		int32 UVOffset = -1;
		int32 NormalOffset = -1;
		int32 TangentOffset = -1;
		int32 BiTangentOffset = -1;
		int32 ColorOffset = -1;


		Ry::ArrayList<VertexAttrib> Attributes;

		int32 FindAttribOffset(const VertexAttrib& InAttrib)
		{
			int32 Offset = 0;
			bool bFound = false;

			for(const VertexAttrib& Attrib : Attributes)
			{
				if(Attrib.Name == InAttrib.Name && Attrib.Size == InAttrib.Size)
				{
					bFound = true;
					break;
				}
				else
				{
					Offset += Attrib.Size;
				}
			}

			if(bFound)
			{
				return Offset;
			}
			else
			{
				return -1;
			}
		}

		void CalculateParams()
		{
			ElementCount = 0;
			
			for (int32 i = 0; i < Attributes.GetSize(); i++)
			{
				VertexAttrib& Attrib = Attributes[i];

				if (Attrib == Ry::pos)
				{
					PosOffset = ElementCount;
				}

				if (Attrib == Ry::normal)
				{
					NormalOffset = ElementCount;
				}

				if (Attrib == Ry::uv)
				{
					UVOffset = ElementCount;
				}

				if (Attrib == Ry::tangent)
				{
					TangentOffset = ElementCount;
				}

				if (Attrib == Ry::bitangent)
				{
					BiTangentOffset = ElementCount;
				}

				if (Attrib == Ry::color)
				{
					ColorOffset = ElementCount;
				}

				ElementCount += Attrib.Size;
			}
		}


	};

	/**
	* Stores a vertex attribute with its relative offset in a series of vertex attributes.
	*/
	struct RENDERING_MODULE VertexAttribPtr
	{
		VertexAttrib attrib;
		int32 offset;
	};

	// position
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P[1];
	
	// position, color
	// position, uv
	// position, normal
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1C[2];
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1UV[2];
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1N[2];

	// position, uv, color
	// position, color, uv
	// position, uv, normal
	// position, normal, uv
	// position, color, normal
	// position, normal, color
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1UV1C[3];
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1C1UV[3];
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1UV1N[3];
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1N1UV[3];
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1C1N[3];
	// extern RENDERING_MODULE VertexAttrib ATTRIB_ARRAY_1P1N1C[3];

	extern RENDERING_MODULE VertexFormat VF1P;
	extern RENDERING_MODULE VertexFormat VF1P1C;
	extern RENDERING_MODULE VertexFormat VF1P1UV;
	extern RENDERING_MODULE VertexFormat VF1P1UV1C;
	extern RENDERING_MODULE VertexFormat VF1P1C1UV;
	extern RENDERING_MODULE VertexFormat VF1P1UV1N;
	extern RENDERING_MODULE VertexFormat VF1P1N1UV;
	extern RENDERING_MODULE VertexFormat VF1P1C1N;
	extern RENDERING_MODULE VertexFormat VF1P1N1C;

	extern RENDERING_MODULE VertexFormat VF_P_UV_N_T_Bi;

	template <>
	EXPORT_ONLY inline uint32 Hash(const Ry::Vertex1P1UV1N& Vert)
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

}
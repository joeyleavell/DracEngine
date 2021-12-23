#pragma once

#include "Core/Core.h"
#include "VertexFormat.gen.h"

namespace Ry
{

	/**
	* Defines meta information about a vertex attribute. A vertex attribute is an atomic element of a vertex.
	*/
	struct ARI_MODULE VertexAttrib
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

	/**
	* Stores a vertex attribute with its relative offset in a series of vertex attributes.
	*/
	struct ARI_MODULE VertexAttribPtr
	{
		VertexAttrib attrib;
		int32 offset;
	};

	extern ARI_MODULE VertexAttrib pos;
	extern ARI_MODULE VertexAttrib color;
	extern ARI_MODULE VertexAttrib uv;
	extern ARI_MODULE VertexAttrib normal;
	extern ARI_MODULE VertexAttrib tangent;
	extern ARI_MODULE VertexAttrib bitangent;

	class ARI_MODULE VertexFormat
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

			for (const VertexAttrib& Attrib : Attributes)
			{
				if (Attrib.Name == InAttrib.Name && Attrib.Size == InAttrib.Size)
				{
					bFound = true;
					break;
				}
				else
				{
					Offset += Attrib.Size;
				}
			}

			if (bFound)
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

	extern ARI_MODULE VertexFormat VF1P;
	extern ARI_MODULE VertexFormat VF1P1C;
	extern ARI_MODULE VertexFormat VF1P1UV;
	extern ARI_MODULE VertexFormat VF1P1UV1C;
	extern ARI_MODULE VertexFormat VF1P1C1UV;
	extern ARI_MODULE VertexFormat VF1P1UV1N;
	extern ARI_MODULE VertexFormat VF1P1N1UV;
	extern ARI_MODULE VertexFormat VF1P1C1N;
	extern ARI_MODULE VertexFormat VF1P1N1C;
	extern ARI_MODULE VertexFormat VF_P_UV_N_T_Bi;

}
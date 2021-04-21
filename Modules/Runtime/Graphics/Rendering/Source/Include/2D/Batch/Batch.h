#pragma once

#include "Core/Core.h"
#include "Core/Globals.h"
#include "Data/ArrayList.h"
#include "RyMath.h"
#include "RenderingEngine.h"
#include "Color.h"
#include "Mesh.h"
#include "Data/ArrayList.h"
#include <cstdarg>
#include "Mesh2.h"

#define TEXT_VERT "/Engine/Shaders/Vertex/font.glv"
#define TEXT_FRAG "/Engine/Shaders/Fragment/font.glf"

#define MAX_VERTS 50000
#define MAX_INDICES 300000

#define CHECK_FLUSH_PRIM(primitive, needed_verts, needed_indices)  \
if (primitive != this->prim || mesh->GetMeshData()->GetVertexCount() + needed_verts >= MAX_VERTS || mesh->GetMeshData()->GetIndexCount() + needed_indices >= MAX_INDICES) \
{ \
	flush(); \
	this->prim = primitive; \
} \


#define CHECK_FLUSH_TEXTURE(text)  \
if (text != this->texture || mesh->GetMeshData()->GetVertexCount() + 4 >= MAX_VERTS || mesh->GetMeshData()->GetIndexCount() + 6 >= MAX_INDICES) \
{ \
	flush(); \
	this->texture = text; \
} \

#define CHECK_ENDED() \
if(began) \
{ \
	std::cerr << "Must invoke end() before invoking a subsequent begin()" << std::endl; \
	return; \
}

#define CHECK_BEGAN() \
if(!began) \
{ \
	std::cerr << "Must invoke begin() before calling rendering functions" << std::endl; \
	return; \
}

#define ADD_LINE(v1, v2) \
uint32 i0=mesh->GetMeshData()->GetVertexCount(); \
mesh->GetMeshData()->AddVertex(&v1); \
mesh->GetMeshData()->AddVertex(&v2); \
mesh->GetMeshData()->AddIndex(i0); \
mesh->GetMeshData()->AddIndex(i0+1); \

#define ADD_TRI(v1, v2, v3) \
uint32 i0 = mesh->GetMeshData()->GetVertexCount(); \
mesh->GetMeshData()->AddVertex(&v1); \
mesh->GetMeshData()->AddVertex(&v2); \
mesh->GetMeshData()->AddVertex(&v3); \
mesh->GetMeshData()->AddTriangle(i0, i0+1, i0+2); \

#define ADD_QUAD(Mesh, v1, v2, v3, v4) \
uint32 i0 = Mesh->GetMeshData()->GetVertexCount(); \
Mesh->GetMeshData()->AddVertex(&v1); \
Mesh->GetMeshData()->AddVertex(&v2); \
Mesh->GetMeshData()->AddVertex(&v3); \
Mesh->GetMeshData()->AddVertex(&v4); \
Mesh->GetMeshData()->AddTriangle(i0, i0+1, i0+2); \
Mesh->GetMeshData()->AddTriangle(i0+2, i0+3, i0+0); \

namespace Ry
{
	class RenderPass2;
	class Texture2;
	class Text;
	class Color;
	class Mesh;
	class Camera;
	class Shader;
	class Texture;
	class BitmapFont;
	class Pipeline2;
	class SwapChain;
	class Shader2;
	class ResourceSet;
	class ResourceSetDescription;
	class RenderingCommandBuffer2;

	struct RENDERING_MODULE BatchItem
	{
		Ry::ArrayList<float> Data;
		Ry::ArrayList<uint32> Indices;
		int32 VertexCount;
		int32 IndexCount;

		BatchItem()
		{
			this->VertexCount = 0;
			this->IndexCount = 0;
		}

		void Clear()
		{
			Data.Clear();
			Indices.Clear();

			// Reset vertex and index count back to zero
			this->VertexCount = 0;
			this->IndexCount = 0;
		}

		void AddVertex(Ry::VertexFormat& Format, Ry::Vertex* Vert)
		{
			float VertData[30];
			Vert->Pack(VertData);

			for(int32 Element = 0; Element < Format.element_count; Element++)
			{
				Data.Add(VertData[Element]);
			}

			VertexCount++;
		}

		void AddVertex1P1C(float X, float Y, float Z, float R, float G, float B, float A)
		{
			Data.Add(X);
			Data.Add(Y);
			Data.Add(Z);
			Data.Add(R);
			Data.Add(G);
			Data.Add(B);
			Data.Add(A);

			VertexCount++;
		}

		void AddVertex1P1UV1C(float X, float Y, float Z, float U, float V, float R, float G, float B, float A)
		{
			Data.Add(X);
			Data.Add(Y);
			Data.Add(Z);
			Data.Add(U);
			Data.Add(V);
			Data.Add(R);
			Data.Add(G);
			Data.Add(B);
			Data.Add(A);

			VertexCount++;
		}

		void AddIndex(int32 Index)
		{
			Indices.Add(Index);

			IndexCount++;
		}

		void AddTriangle(int32 Index1, int32 Index2, int32 Index3)
		{
			Indices.Add(Index1);
			Indices.Add(Index2);
			Indices.Add(Index3);

			IndexCount += 3;
		}
		
	};

	struct RENDERING_MODULE BatchItemSet
	{
		Ry::ArrayList<Ry::SharedPtr<BatchItem>> Items;

		void AddItem(Ry::SharedPtr<BatchItem> Item)
		{
			Items.Add(Item);
		}
	};

	RENDERING_MODULE Ry::SharedPtr<BatchItem> MakeItem();
	RENDERING_MODULE Ry::SharedPtr<BatchItemSet> MakeItemSet();
	RENDERING_MODULE void BatchRectangle(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Width, float Height, float Depth);
	RENDERING_MODULE void BatchSubArc(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float InnerRadius, float OuterRadius, float Theta0, float Theta1, int32 Segments, float Depth);
	RENDERING_MODULE void BatchArc(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Radius, float Theta0, float Theta1, int32 Segments, float Depth);
	RENDERING_MODULE void BatchStyledBox(Ry::SharedPtr<BatchItemSet> Item, float X, float Y, float W, float H, const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize, float Depth);
	RENDERING_MODULE void BatchTexture(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Tint, float X, float Y, float U, float V, float UVWidth, float UVHeight, float OriginX, float OriginY, float Width, float Height, float Depth);
	RENDERING_MODULE void BatchText(Ry::SharedPtr<BatchItemSet> Item, const Ry::Color& Color, BitmapFont* Font, const Ry::String& Text, float XPosition, float YPosition, float LineWidth);

	class RENDERING_MODULE Batch
	{
	public:

		Batch(Ry::SwapChain* Target, Ry::RenderPass2* ParentPass, const VertexFormat& Format = VF1P1C, Ry::Shader2* Shad = nullptr, bool bTexture = false);
		
		void AddItem(Ry::SharedPtr<BatchItem> Item);
		void RemoveItem(Ry::SharedPtr<BatchItem> Item);
		void AddItemSet(Ry::SharedPtr<BatchItemSet> ItemSet);
		void RemoveItemSet(Ry::SharedPtr<BatchItemSet> ItemSet);
		
		void SetView(const Matrix4& View);
		void Resize(int32 Width, int32 Height);
		void SetProjection(const Matrix4& Proj);
		//void SetShader(const Ry::String& ShaderName);
		void Camera(const Camera* Cam);
		void Update();
		void SetTexture(Texture2* Texture);
		bool Render();

		void SetRenderPass(RenderPass2* ParentRenderPass);

		Ry::RenderingCommandBuffer2* GetCommandBuffer();
		
	private:

		bool bNeedsRecord = false;

		void CreateResources(SwapChain* Swap);
		void CreatePipeline(const VertexFormat& Format, Ry::SwapChain* SwapChain, Ry::Shader2* Shad);

		void RecordCommands();

		//Shader* Shad;
		Texture2* Tex;
		Matrix4 Projection;
		Matrix4 View;

		Mesh2* BatchMesh;

		int LastIndexCount = -1;
		
		Ry::RenderingCommandBuffer2* CommandBuffer;
		Ry::RenderPass2* ParentPass;
		
		// Texture resources
		Ry::ResourceSetDescription* TextureResDesc;
		Ry::ResourceSet* TextureRes;

		// Scene resources
		Ry::ResourceSetDescription* SceneResDesc;
		Ry::ResourceSet* SceneRes;

		Ry::Pipeline2* Pipeline;

		Ry::ArrayList<ResourceSet*> ResourceSets;

		// Maps depth to a list of items
		Ry::ArrayList<Ry::SharedPtr<BatchItem>> Items;
		Ry::ArrayList<Ry::SharedPtr<BatchItemSet>> ItemSets;

		bool bUseTexture;
	};


	class RENDERING_MODULE TextBatch
	{
	public:
		TextBatch(Ry::SwapChain* Target, Shader2* Shad = nullptr);

		void Begin();

		void RenderTextBodyWrapped(const Ry::Text& Text, float XPosition, float YPosition, float LineWidth);

		void End();
		void Flush();

		void SetFont(BitmapFont* Font);
		void SetColor(const Ry::Vector3& Color);

	private:

		void CreateResources(SwapChain* Swap);
		void CreatePipeline(const VertexFormat& Format, Ry::SwapChain* SwapChain, Ry::Shader2* Shad);
		void RecordCommands();

		bool began;
		BitmapFont* Font;
		Ry::Vector3 Color;
		//Shader* FontShader;
		
		Mesh2* FontMesh;

		Ry::ResourceSet* Resources[2];

		// Texture resources
		Ry::ResourceSetDescription* TextureResDesc;
		Ry::ResourceSet* TextureRes;

		// Scene resources
		Ry::ResourceSetDescription* SceneResDesc;
		Ry::ResourceSet* SceneRes;

		Ry::RenderingCommandBuffer2* CommandBuffer;

		Ry::Pipeline2* Pipeline;


	};
	
}
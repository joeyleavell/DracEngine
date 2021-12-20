#pragma once

#include "Core/Core.h"
#include "Core/Globals.h"
#include "Data/ArrayList.h"
#include "RyMath.h"
#include "RenderingEngine.h"
#include "Color.h"
#include "Data/ArrayList.h"
#include <cstdarg>
#include "Mesh.h"
#include "Interface/Pipeline.h"
#include "Data/Set.h"

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
	struct Transform2D;
	struct BatchPipeline;
	class RenderPass;
	class Texture;
	class Text;
	class Color;
	class Mesh;
	class Camera;
	class Shader;
	class Texture;
	class BitmapFont;
	class Pipeline;
	class SwapChain;
	class Shader;
	class ResourceSet;
	class ResourceLayout;
	class CommandBuffer;

	struct RENDERING_MODULE TextLine
	{
		Ry::ArrayList<Ry::String> Words;
	};


	struct RENDERING_MODULE PrecomputedTextData
	{
		Ry::ArrayList<Ry::TextLine> Lines;
	};

	RENDERING_MODULE void ComputeTextData(PrecomputedTextData& OutData, const Ry::String Text);

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
			Data.SoftClear();
			Indices.SoftClear();

			// Reset vertex and index count back to zero
			this->VertexCount = 0;
			this->IndexCount = 0;
		}

		void AddVertex(Ry::VertexFormat& Format, Ry::Vertex* Vert)
		{
			float VertData[30];
			Vert->Pack(VertData);

			for(int32 Element = 0; Element < Format.GetElementCount(); Element++)
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


	RENDERING_MODULE void TransformedRect(Ry::Vector3* OutVerts, Ry::Matrix3 Transform, float OriginX, float OriginY, float Width, float Height);
	
	RENDERING_MODULE void BatchRectangleTransform(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, Ry::Matrix3 Transform, float Width, float Height, float OriginX, float OriginY, float Depth);

	RENDERING_MODULE void BatchRectangle(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Width, float Height, float Depth);
	RENDERING_MODULE void BatchHollowRectangle(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Width, float Height, float Thickness, float Depth);
	RENDERING_MODULE void BatchSubArc(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float InnerRadius, float OuterRadius, float Theta0, float Theta1, int32 Segments, float Depth);
	RENDERING_MODULE void BatchArc(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Radius, float Theta0, float Theta1, int32 Segments, float Depth);
	RENDERING_MODULE void BatchStyledBox(Ry::SharedPtr<BatchItemSet> Item, float X, float Y, float W, float H, const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize, float Depth);
	RENDERING_MODULE void BatchTexture(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Tint, float X, float Y, float U, float V, float UVWidth, float UVHeight, float OriginX, float OriginY, float Width, float Height, float Depth);
	RENDERING_MODULE void BatchText(Ry::SharedPtr<BatchItemSet> Item, const Ry::Color& Color, BitmapFont* Font, const PrecomputedTextData& TextData, float XPosition, float YPosition, float LineWidth);

	RENDERING_MODULE void BatchTextureTransform(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Tint, Ry::Matrix3 Transform, float U, float V, float UVWidth, float UVHeight, float OriginX, float OriginY, float Width, float Height, float Depth);

	struct RENDERING_MODULE PipelineState
	{
		// Used for referencing this specific state ID in the future.
		Ry::String StateID;
		RectScissor Scissor;

		bool operator==(const PipelineState& Other) const
		{
			return StateID == Other.StateID;
		}
	};

	struct RENDERING_MODULE BatchGroup
	{
		BatchPipeline* OwningPipeline;

		PipelineState State;
		Texture* Text = nullptr;
		Mesh* BatchMesh = nullptr;
		Ry::ArrayList<ResourceSet*> ResourceSets;
		
		Ry::OASet<Ry::SharedPtr<BatchItem>> Items;
		Ry::OASet<Ry::SharedPtr<BatchItemSet>> ItemSets;

		// Batch group specific resources
		ResourceSet* TextureResources;

		int LastIndexCount = -1;

		~BatchGroup()
		{
			delete BatchMesh;
		}

	};

	struct RENDERING_MODULE BatchPipeline
	{
		Ry::Pipeline* Pipeline;
		VertexFormat Format;

		// Pipeline/scene specific resources
		ResourceSet* SceneResources;

		// Resource layouts
		const Ry::ResourceLayout* SceneResDesc;
		const Ry::ResourceLayout* TextureResDesc;
	};

	struct RENDERING_MODULE BatchLayer
	{
		int32 Depth = 0;
		Ry::CommandBuffer* CommandBuffer = nullptr;		
		Ry::OAHashMap<BatchPipeline*, Ry::ArrayList<BatchGroup*>> Groups;
		bool bNeedsRecord = false;

		BatchLayer():
		Groups(10) // Small table size to increase load factor
		{
			
		}

		~BatchLayer()
		{
			Ry::OAPairIterator<BatchPipeline*, Ry::ArrayList<BatchGroup*>> Itr = Groups.CreatePairIterator();
			while(Itr)
			{
				for (BatchGroup* Group : Itr.GetValue())
					delete Group;				
				++Itr;
			}

			Groups.Clear();
		}
	};

	class RENDERING_MODULE Batch
	{
	public:

		// Used in cases of off-screen rendering
		Batch(RenderPass* RP, int32 RenderTargetWidth, int32 RenderTargetHeight);

		/**
		 * This constructor is used when specifying a batch that is dependent on the swap chain.
		 *
		 * If the specified render pass is null, the default swap chain pass will be used.
		 */
		Batch(Ry::SwapChain* Target, Ry::RenderPass* Pass = nullptr);

		void AddPipeline(Ry::String Name, Ry::String Shader);

		void AddItem(Ry::SharedPtr<BatchItem> Item, Ry::String PipelineId, PipelineState State, Texture* Texture = nullptr, int32 Layer = -1);
		void AddItemSet(Ry::SharedPtr<BatchItemSet> ItemSet, Ry::String PipelineId, PipelineState State, Texture* Texture = nullptr, int32 Layer = -1);
		
		void RemoveItem(Ry::SharedPtr<BatchItem> Item);
		void RemoveItemSet(Ry::SharedPtr<BatchItemSet> ItemSet);

		void Clear();
		
		void SetView(const Matrix4& View);
		void Resize(int32 Width, int32 Height);
		void SetProjection(const Matrix4& Proj);
		void Camera(const Camera* Cam);
		void Update();
		bool Render();

		void UpdatePipelineState(const PipelineState& State);

		int32 GetLayerCount() const;

		void SetRenderPass(RenderPass* ParentRenderPass);

		Ry::CommandBuffer* GetCommandBuffer(int32 Layer);

		// TODO: this function may be needed if memory usage gets too high
		void DeleteEmptyGroups();
		
	private:

		void Init();

		int32 RenderTargetWidth;
		int32 RenderTargetHeight;

		void CreateLayersIfNeeded(int32 Index);

		// Finds a batch group for given state information
		BatchGroup* FindOrCreateBatchGroup(Ry::String PipelineId, PipelineState State, Texture* Text, int32 Layer);
		BatchGroup* FindBatchGroup(Texture* Text, PipelineState State, int32 Layer);

		void RecordCommands(int32 Layer);

		Matrix4 Projection;
		Matrix4 View;
		
		Ry::RenderPass* ParentPass;
		Ry::SwapChain* Swap = nullptr;
		
		Ry::Map<Ry::String, Ry::BatchPipeline*> Pipelines;
		Ry::ArrayList<BatchLayer*> Layers;
	};
	
}
#include "2D/Batch/Batch.h"
#include "Mesh.h"
#include "Camera.h"
#include "MeshData.h"
#include "Font.h"
#include "RenderingEngine.h"
#include "Color.h"
#include "Camera.h"
#include "RyMath.h"
#include "Core/Globals.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include "Interface2/Pipeline.h"
#include "SwapChain.h"
#include "Interface2/RenderAPI.h"
#include "Interface2/RenderingResource.h"
#include "Interface2/RenderCommand.h"
#include "Interface2/VertexArray2.h"
#include "Interface2/Texture2.h"

namespace Ry
{


	Ry::SharedPtr<BatchItem> MakeItem()
	{
		return Ry::MakeShared<>(new BatchItem);
	}

	Ry::SharedPtr<BatchItemSet> MakeItemSet()
	{
		return Ry::MakeShared<>(new BatchItemSet);
	}

	void BatchRectangle(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Width, float Height, float Depth)
	{
		// Clear the previous item data
		Item->Clear();

		Item->AddVertex1P1C(X, Y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X, Y + Height, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width, Y + Height, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width, Y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

		Item->AddTriangle(0, 1, 2);
		Item->AddTriangle(2, 3, 0);
	}

	void BatchSubArc(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float InnerRadius, float OuterRadius, float Theta0, float Theta1, int32 Segments, float Depth)
	{
		if (Segments < 3)
		{
			Ry::Log->LogError("Must have at least 3 segments for a circle"); // TODO: change this to an assert
			return;
		}

		// Clear the previous item data
		Item->Clear();

		float MinTheta = Theta0 < Theta1 ? Theta0 : Theta1;
		float MaxTheta = Theta0 > Theta1 ? Theta0 : Theta1;
		float ThetaRange = MaxTheta - MinTheta;


		// Create first radial vertices
		Item->AddVertex1P1C(X + cos(MinTheta) * InnerRadius, Y + sin(MinTheta) * InnerRadius, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + cos(MinTheta) * OuterRadius, Y + sin(MinTheta) * OuterRadius, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

		float AngleIncrement = ThetaRange / Segments;
		for (uint32 Radial = 0; Radial <= (uint32) Segments; Radial++)
		{
			float RelativeXUnscaled = cos(AngleIncrement * (Radial + 1) + MinTheta);
			float RelativeYUnscaled = sin(AngleIncrement * (Radial + 1) + MinTheta);

			Item->AddVertex1P1C(X + RelativeXUnscaled * InnerRadius, Y + RelativeYUnscaled * InnerRadius, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
			Item->AddVertex1P1C(X + RelativeXUnscaled * OuterRadius, Y + RelativeYUnscaled * OuterRadius, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

			int32 PrevInnerRadialIndex = Radial * 2 + 0;
			int32 PrevOuterRadialIndex = Radial * 2 + 1;
			int32 InnerRadialIndex = (Radial + 1) * 2 + 0;
			int32 OuterRadialIndex = (Radial + 1) * 2 + 1;

			// Build the mesh triangle fan style
			Item->AddTriangle(PrevOuterRadialIndex, PrevInnerRadialIndex, InnerRadialIndex);
			Item->AddTriangle(PrevOuterRadialIndex, InnerRadialIndex, OuterRadialIndex);
		}
	}

	void BatchArc(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Radius, float Theta0, float Theta1, int32 Segments, float Depth)
	{
		if (Segments < 3)
		{
			Ry::Log->LogError("Must have at least 3 segments for a circle");
			return;
		}

		// Clear the previous item data
		Item->Clear();

		float MinTheta = Theta0 < Theta1 ? Theta0 : Theta1;
		float MaxTheta = Theta0 > Theta1 ? Theta0 : Theta1;
		float ThetaRange = MaxTheta - MinTheta;
		float AngleIncrement = ThetaRange / Segments;

		// Add the center vertex and first radial vertex
		Item->AddVertex1P1C(X, Y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Center vertex
		Item->AddVertex1P1C(X + cos(MinTheta) * Radius, Y + sin(MinTheta) * Radius, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha); // First radial vertex

		for (uint32 RadialIndex = 0; RadialIndex <= (uint32) Segments; RadialIndex++)
		{
			float x_o = X + cos(AngleIncrement * (RadialIndex + 1) + MinTheta) * Radius;
			float y_o = Y + sin(AngleIncrement * (RadialIndex + 1) + MinTheta) * Radius;

			// Add the next triangle
			Item->AddVertex1P1C(x_o, y_o, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
			//Item->AddTriangle(0, RadialIndex /*Previous vertex*/, 1 + RadialIndex /*Current vertex*/);
			Item->AddTriangle(1 + RadialIndex, RadialIndex /*Previous vertex*/ /*Current vertex*/, 0);

		}
	}

	void BatchStyledBox(Ry::SharedPtr<BatchItemSet> Item, float X, float Y, float W, float H, const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize, float Depth)
	{
		// Reset all data in this batch item
		Item->Items.Clear();

		// Constant for the resolution to use when drawing radial features
		const int32 SEGMENTS = 20;

		// Render the center
		Ry::SharedPtr<BatchItem> CenterRect1 = MakeItem();
		Ry::SharedPtr<BatchItem>  CenterRect2 = MakeItem();

		BatchRectangle(CenterRect1, BackgroundColor, X, Y + BorderRadius, W, H - 2 * BorderRadius, Depth);
		BatchRectangle(CenterRect2, BackgroundColor, X + BorderRadius, Y, W - 2 * BorderRadius, H, Depth);

		Item->AddItem(CenterRect1);
		Item->AddItem(CenterRect2);

		if (BorderRadius > 0)
		{
			// Render the edges
			Ry::SharedPtr<BatchItem> ArcTopLeft = MakeItem();
			Ry::SharedPtr<BatchItem> ArcTopRight = MakeItem();
			Ry::SharedPtr<BatchItem> ArcBottomLeft = MakeItem();
			Ry::SharedPtr<BatchItem> ArcBottomRight = MakeItem();

			BatchArc(ArcTopLeft, BackgroundColor, X + BorderRadius, Y + BorderRadius, (float) BorderRadius, PI, 3 * PI / 2, SEGMENTS, Depth);
			BatchArc(ArcBottomLeft, BackgroundColor, X + BorderRadius, Y + H - BorderRadius, (float) BorderRadius, PI / 2, PI, SEGMENTS, Depth);
			BatchArc(ArcBottomRight, BackgroundColor, X + W - BorderRadius, Y + H - BorderRadius, (float)BorderRadius, 0, PI / 2, SEGMENTS, Depth);
			BatchArc(ArcTopRight, BackgroundColor, X + W - BorderRadius, Y + BorderRadius, (float) BorderRadius, 3 * PI / 2, 2 * PI, SEGMENTS, Depth);

			Item->AddItem(ArcTopLeft);
			Item->AddItem(ArcTopRight);
			Item->AddItem(ArcBottomLeft);
			Item->AddItem(ArcBottomRight);
		}

		if (BorderSize > 0)
		{
			// Render the side borders
			Ry::SharedPtr<BatchItem> LeftBorder = MakeItem();
			Ry::SharedPtr<BatchItem> RightBorder = MakeItem();
			Ry::SharedPtr<BatchItem> TopBorder = MakeItem();
			Ry::SharedPtr<BatchItem> BottomBorder = MakeItem();

			BatchRectangle(LeftBorder, BorderColor, X - BorderSize, Y + BorderRadius, (float) BorderSize, H - 2 * BorderRadius, Depth);
			BatchRectangle(BottomBorder, BorderColor, X + BorderRadius, Y + H, W - 2.0f * BorderRadius, (float) BorderSize, Depth);
			BatchRectangle(RightBorder, BorderColor, X + W, Y + BorderRadius, (float) BorderSize, H - 2.0f * BorderRadius, Depth);
			BatchRectangle(TopBorder, BorderColor, X + BorderRadius, Y - BorderSize, W - 2.0f * BorderRadius, (float) BorderSize, Depth);

			Item->AddItem(LeftBorder);
			Item->AddItem(RightBorder);
			Item->AddItem(TopBorder);
			Item->AddItem(BottomBorder);

			// Render the radial borders
			if (BorderRadius > 0)
			{
				Ry::SharedPtr<BatchItem> SubArcTopLeft = MakeItem();
				Ry::SharedPtr<BatchItem> SubArcTopRight = MakeItem();
				Ry::SharedPtr<BatchItem> SubArcBottomLeft = MakeItem();
				Ry::SharedPtr<BatchItem> SubArcBottomRight = MakeItem();

				// Draw subarcs for the border
				BatchSubArc(SubArcTopLeft, BorderColor, X + BorderRadius, Y + BorderRadius, (float) BorderRadius, (float) BorderRadius + BorderSize, PI, 3 * PI / 2, SEGMENTS, Depth);
				BatchSubArc(SubArcBottomLeft, BorderColor, X + BorderRadius, Y + H - BorderRadius, (float) BorderRadius, (float) BorderRadius + BorderSize, PI / 2, PI, SEGMENTS, Depth);
				BatchSubArc(SubArcBottomRight, BorderColor, X + W - BorderRadius, Y + H - BorderRadius, (float) BorderRadius, (float) BorderRadius + BorderSize, 0, PI / 2, SEGMENTS, Depth);
				BatchSubArc(SubArcTopRight, BorderColor, X + W - BorderRadius, Y + BorderRadius, (float) BorderRadius, (float) BorderRadius + BorderSize, 3 * PI / 2, 2 * PI, SEGMENTS, Depth);

				Item->AddItem(SubArcTopLeft);
				Item->AddItem(SubArcTopRight);
				Item->AddItem(SubArcBottomLeft);
				Item->AddItem(SubArcBottomRight);
			}
		}
	}

	void BatchTexture(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Tint, float X, float Y, float U, float V, float UVWidth, float UVHeight, float OriginX, float OriginY, float Width, float Height, float Depth)
	{
		Item->Clear();

		Item->AddVertex1P1UV1C(0 - OriginX + X, 0 - OriginY + Y, Depth, U, V, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C(Width - OriginX + X, 0 - OriginY + Y, Depth, U + UVWidth, V, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C(Width - OriginX + X, -Height - OriginY + Y, Depth, U + UVWidth, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C(0 - OriginX + X, -Height - OriginY + Y, Depth, U, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);

		Item->AddTriangle(0, 1, 2);
		Item->AddTriangle(2, 3, 0);
	}

	void BatchText(Ry::SharedPtr<BatchItemSet> ItemSet, const Ry::Color& Color, BitmapFont* Font, const Ry::String& Text, float XPosition, float YPosition, float LineWidth)
	{
		ItemSet->Items.Clear();

		// The amount of spaces in a tab
		const int32 TAB_SPACES = 4;
		const int32 SPACE_ADVANCE = Font->GetGlyph(static_cast<int32>(' '))->AdvanceWidth;

		// Establish origin
		float CurX = XPosition;
		float CurY = YPosition + (Font->GetAscent());

		// Get the advance width of the space character

		StringView* Lines = nullptr;
		int32 LineCount = Text.split("\n", &Lines);

		for (int32 Line = 0; Line < LineCount; Line++)
		{
			StringView* Words = nullptr;
			int32 WordCount = Lines[Line].split(" ", &Words);

			for (int32 Word = 0; Word < WordCount; Word++)
			{
				const Ry::StringView& CurrentWord = Words[Word];
				float TextWidth = Font->MeasureWidth(CurrentWord);

				// Wrap text to next line
				if (CurX + TextWidth > XPosition + LineWidth)
				{
					// Reset cursor
					CurX = XPosition;
					CurY += Font->GetAscent() - Font->GetDescent() + Font->GetLineGap();
				}

				for (uint32 WordChar = 0; WordChar < CurrentWord.getSize(); WordChar++)
				{
					char Character = CurrentWord[WordChar];
					int32 Codepoint = static_cast<int32>(Character);
					BitmapGlyph* BmpGlyph = Font->GetGlyph(Codepoint);

					// Generate texture coordinates for the glyph
					if (!BmpGlyph)
					{
						if (Codepoint == '\t')
						{
							CurX += TAB_SPACES * SPACE_ADVANCE;
						}
					}
					else
					{
						
						int32 RasterWidth = BmpGlyph->Raster->GetWidth();
						int32 RasterHeight = BmpGlyph->Raster->GetHeight();
						int32 OriginX = BmpGlyph->OriginX;
						int32 OriginY = BmpGlyph->OriginY;
						int32 AdvanceWidth = BmpGlyph->AdvanceWidth;

						float U = BmpGlyph->SheetX / float(FONT_BITMAP_SIZE);
						float V = BmpGlyph->SheetY / float(FONT_BITMAP_SIZE);
						float UVW = RasterWidth / float(FONT_BITMAP_SIZE);
						float UVH = RasterHeight / float(FONT_BITMAP_SIZE);

						float TL_X = CurX + OriginX;
						float TL_Y = CurY + OriginY;

						float TR_X = TL_X + RasterWidth;
						float TR_Y = TL_Y;

						float BL_X = TL_X;
						float BL_Y = TL_Y + RasterHeight;

						float BR_X = TL_X + RasterWidth;
						float BR_Y = TL_Y + RasterHeight;

						// Create a batch item for this glyph
						Ry::SharedPtr<BatchItem> GlyphItem = MakeItem();
						GlyphItem->AddVertex1P1UV1C(TL_X, TL_Y, 0.0f, U, V, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Top left
						GlyphItem->AddVertex1P1UV1C(TR_X, TR_Y, 0.0f, U + UVW, V, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Top right
						GlyphItem->AddVertex1P1UV1C(BR_X, BR_Y, 0.0f, U + UVW, V + UVH, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Bottom right
						GlyphItem->AddVertex1P1UV1C(BL_X, BL_Y, 0.0f, U, V + UVH, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Bottom left
						GlyphItem->AddTriangle(2, 1, 0);
						GlyphItem->AddTriangle(0, 3, 2);
						ItemSet->AddItem(GlyphItem);

						// Advance the cursor
						CurX += AdvanceWidth;

						// Kerning lookup
						if (WordChar < CurrentWord.getSize() - 1)
						{
							int32 NextCodepoint = static_cast<int32>(CurrentWord[WordChar + 1]);
							if (Font->GetGlyph(NextCodepoint))
							{
								int32 KernAmount = *BmpGlyph->KerningTable.get(NextCodepoint);
								CurX += KernAmount;
							}
						}
						
					}

				}

				if(Word < WordCount - 1)
				{
					CurX += SPACE_ADVANCE;
				}
			}

			// Move cursor to the next line
			CurY += Font->GetAscent() - Font->GetDescent() + Font->GetLineGap();
			CurX = XPosition;
		}
	}
	
	Batch::Batch(Ry::SwapChain* Target, const VertexFormat& Format, Ry::Shader2* Shad, bool bTexture)
	{
		// Initialize dynamic mesh
		BatchMesh = new Ry::Mesh2(Format);
		
		Ry::Shader2* ShaderToUse = Shad;
		if(!ShaderToUse)
		{
			ShaderToUse = Ry::GetShader("Shape");
		}

		this->bUseTexture = bTexture;
		this->Tex = nullptr;


		//BatchMesh->GetMeshData()->SetShaderAll(ShaderToUse);

		// Initialize resources
		CreateResources(Target);
		
		// Initialize pipeline
		CreatePipeline(Format, Target, ShaderToUse);

		// Create command buffer
		CommandBuffer = Ry::NewRenderAPI->CreateCommandBuffer(Target);
		//RecordCommands();

		View = Ry::id4();
		Projection = Ry::ortho4(0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight(), 0.0f, -1.0f, 1.0f);

		// Create rendering pipeline
	}

	void Batch::AddItem(Ry::SharedPtr<BatchItem> Item)
	{
		Items.Add(Item);
	}

	void Batch::RemoveItem(Ry::SharedPtr<BatchItem> Item)
	{
		Items.Remove(Item);
	}

	void Batch::AddItemSet(Ry::SharedPtr<BatchItemSet> Item)
	{
		ItemSets.Add(Item);
	}

	void Batch::RemoveItemSet(Ry::SharedPtr<BatchItemSet> Item)
	{
		ItemSets.Remove(Item);
	}

	void Batch::SetView(const Matrix4& View)
	{
		this->View = View;
	}

	void Batch::Resize(int32 Width, int32 Height)
	{
		Projection = Ry::ortho4(0, (float) Width, (float)Height, 0, -1, 1);
	}

	void Batch::SetProjection(const Matrix4& Proj)
	{
		this->Projection = Proj;
	}

	void Batch::Camera(const Ry::Camera* Cam)
	{
		this->View = Cam->get_view();
		this->Projection = Cam->get_proj();
	}

	void Batch::Update()
	{
		BatchMesh->Reset();

		auto AddItem = [this](Ry::SharedPtr<BatchItem> Item)
		{
			uint32 BaseIndex = BatchMesh->GetMeshData()->GetVertexCount();

			// Add all vertices and indices
			BatchMesh->GetMeshData()->AddVertexRaw(Item->Data.GetData(), (int32)Item->Data.GetSize(), Item->VertexCount);
			BatchMesh->GetMeshData()->AddIndexRaw(Item->Indices.GetData(), (int32)Item->Indices.GetSize(), BaseIndex);
		};

		// Rebuild the mesh
		std::for_each(Items.begin(), Items.end(), AddItem);
		std::for_each(ItemSets.begin(), ItemSets.end(), [AddItem](Ry::SharedPtr<BatchItemSet> ItemSet)
			{
				std::for_each(ItemSet->Items.begin(), ItemSet->Items.end(), AddItem);
			});

		BatchMesh->Update();

		int CurrentIndexCount = BatchMesh->GetVertexArray()->GetIndexCount();
		if (LastIndexCount < 0 || LastIndexCount != CurrentIndexCount)
		{
			bNeedsRecord = true;
			LastIndexCount = CurrentIndexCount;
		}
	}

	void Batch::SetTexture(Ry::Texture2* Texture)
	{
		if(Texture != this->Tex)
		{
			this->Tex = Texture;

			// Bind the texture to texture resources
			TextureRes->BindTexture("BatchTexture", Texture);

			// Need to re-record buffer since we updated descriptor set
			bNeedsRecord = true;
		}
		
	}

	void Batch::Render()
	{
		// Update uniform
		// Todo: change this to a push constant and only change it if needed
		SceneRes->SetMatConstant("Scene", "ViewProj", Projection * View);

		// Update resource resets and re-record command buffers if needed
		for(ResourceSet* Set : ResourceSets)
		{
			if(Set->Update())
			{
				bNeedsRecord = true;
			}
		}

		if (bNeedsRecord)
		{
			// Re-record commands
			RecordCommands();
			bNeedsRecord = false;
		}

		// Submit command buffer, assume we're in a valid context
		CommandBuffer->Submit();
	}

	void Batch::CreateResources(SwapChain* Swap)
	{
		SceneResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ ShaderStage::Vertex}, 0);
		SceneResDesc->AddConstantBuffer(0, "Scene", {
			DeclPrimitive(Float4x4, "ViewProj")
		});
		SceneResDesc->CreateDescription();


		// Create actual resource set now
		SceneRes = Ry::NewRenderAPI->CreateResourceSet(SceneResDesc, Swap);
		SceneRes->CreateBuffer();

		ResourceSets.Add(SceneRes);

		if (bUseTexture)
		{
			TextureResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ ShaderStage::Fragment }, 1);
			TextureResDesc->AddTextureBinding(0, "BatchTexture");
			TextureResDesc->CreateDescription();

			// Create texture resources
			// Bind the default texture to start with
			TextureRes = Ry::NewRenderAPI->CreateResourceSet(TextureResDesc, Swap);
			TextureRes->BindTexture("BatchTexture", DefaultTexture);
			TextureRes->CreateBuffer();

			ResourceSets.Add(TextureRes);

			this->Tex = DefaultTexture;

		}

	}

	void Batch::CreatePipeline(const VertexFormat& Format, Ry::SwapChain* SwapChain, Ry::Shader2* Shad)
	{
		
		// Create new pipeline
		Ry::PipelineCreateInfo CreateInfo;
		CreateInfo.ViewportWidth = SwapChain->GetSwapChainWidth();
		CreateInfo.ViewportHeight = SwapChain->GetSwapChainHeight();
		CreateInfo.PipelineShader = Shad;
		CreateInfo.VertFormat = Format;
		CreateInfo.RenderPass = SwapChain->GetDefaultRenderPass();

		// Add resource description to pipeline
		CreateInfo.ResourceDescriptions.Add(SceneResDesc);

		if(bUseTexture)
		{
			CreateInfo.ResourceDescriptions.Add(TextureResDesc);
		}

		Pipeline = Ry::NewRenderAPI->CreatePipeline(CreateInfo);
		Pipeline->CreatePipeline();
	}

	void Batch::RecordCommands()
	{
		CommandBuffer->Reset();
		
		CommandBuffer->BeginCmd();
		{
			CommandBuffer->BeginRenderPass();
			{
				CommandBuffer->SetViewportSize(0, 0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight());
				CommandBuffer->SetScissorSize(0, 0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight());

				// Bind pipeline
				CommandBuffer->BindPipeline(Pipeline);

				// Bind resources
				CommandBuffer->BindResources(ResourceSets.GetData(), ResourceSets.GetSize());

				// Draw the mesh
				CommandBuffer->DrawVertexArrayIndexed(BatchMesh->GetVertexArray(), BatchMesh->GetMeshData()->Sections.get(0)->StartIndex, BatchMesh->GetMeshData()->Sections.get(0)->Count);
				
			}
			CommandBuffer->EndRenderPass();

		}
		CommandBuffer->EndCmd();
	}

	/*
	ShapeBatch::ShapeBatch()
	{
		
	}
	
	ShapeBatch::~ShapeBatch()
	{
		delete shader;
		delete mesh;
	}
	
	void ShapeBatch::begin(DrawMode mode)
	{
		CHECK_ENDED()
	
		this->mode = mode;
		this->began = true;
	}
	
	void ShapeBatch::draw_hollow_circle(float x, float y, float r, uint32 segments)
	{
		CHECK_BEGAN()
		CHECK_FLUSH_PRIM(Primitive::LINE, segments, 2 * segments);
	
		if (segments < 3)
		{
			std::cerr << "must have at least 3 segments for a triangle" << std::endl;
			return;
		}
	
		uint32 i0 = mesh->GetMeshData()->GetVertexCount();
	
		float angle_inc = (2 * PI) / segments;
		for (uint32 i = 0; i < segments; i++)
		{
			float x_o = x + cos(angle_inc * i) * r;
			float y_o = y + sin(angle_inc * i) * r;
			Vertex1P1C vert(x_o, y_o, 0.0f, color.r, color.g, color.b, color.a);
			mesh->GetMeshData()->AddVertex(&vert);
	
			if (i < segments - 1)
			{
				mesh->GetMeshData()->AddIndex(i0 + i);
				mesh->GetMeshData()->AddIndex(i0 + i + 1);
			}
		}
		mesh->GetMeshData()->AddIndex(i0 + segments - 1);
		mesh->GetMeshData()->AddIndex(i0);
	
	}
	
	// void ShapeBatch::draw_filled_circle(float x, float y, float r, uint32 segments)
	// {
	// 	CHECK_BEGAN()
	// 	CHECK_FLUSH_PRIM(Primitive::TRIANGLE, segments + 1, 3 * segments);
	//
	// 	if (segments < 3)
	// 	{
	// 		std::cerr << "must have at least 3 segments for a triangle" << std::endl;
	// 		return;
	// 	}
	//
	// 	uint32 i0 = mesh->GetMeshData()->GetVertexCount();
	//
	// 	// Center vertex.
	// 	Vertex1P1UV1C center(x, y, 0.0f, 0.0f, 0.0f, color.r, color.g, color.b, color.a);
	// 	mesh->GetMeshData()->AddVertex(&center);
	//
	// 	float angle_inc = (2 * PI) / segments;
	// 	for (uint32 i = 0; i < segments; i++)
	// 	{
	// 		float x_o = x + cos(angle_inc * i) * r;
	// 		float y_o = y + sin(angle_inc * i) * r;
	//
	// 		Vertex1P1UV1C vert(x_o, y_o, 0.0f, 0.0f, 0.0f, color.r, color.g, color.b, color.a);
	// 		mesh->GetMeshData()->AddVertex(&vert);
	//
	// 		if (i < segments - 1)
	// 		{
	// 			mesh->GetMeshData()->AddTriangle(i0, i0 + 1 + i, i0 + 2 + i);
	// 		}
	// 	}
	// 	mesh->GetMeshData()->AddTriangle(i0, i0 + segments, i0 + 1);
	// }
	
	void ShapeBatch::draw_rect(float x, float y, float w, float h)
	{
		CHECK_BEGAN()
		CHECK_FLUSH_PRIM(Primitive::TRIANGLE, 4, 6)

	}
	
	void ShapeBatch::draw_hollow_rect(float x, float y, float w, float h)
	{
		CHECK_BEGAN()
		CHECK_FLUSH_PRIM(Primitive::LINE, 4, 6)
	
		Vertex1P1C v1(x, y, 0.0f, color.r, color.g, color.b, color.a);
		Vertex1P1C v2(x, y + h, 0.0f, color.r, color.g, color.b, color.a);
		Vertex1P1C v3(x + w, y + h, 0.0f, color.r, color.g, color.b, color.a);
		Vertex1P1C v4(x + w, y, 0.0f, color.r, color.g, color.b, color.a);
	
		uint32 i0 = mesh->GetMeshData()->GetVertexCount();
		mesh->GetMeshData()->AddVertex(&v1);
		mesh->GetMeshData()->AddVertex(&v2);
		mesh->GetMeshData()->AddVertex(&v3);
		mesh->GetMeshData()->AddVertex(&v4);
		mesh->GetMeshData()->AddLine(i0, i0 + 1);
		mesh->GetMeshData()->AddLine(i0 + 1, i0 + 2);
		mesh->GetMeshData()->AddLine(i0 + 2, i0 + 3);
		mesh->GetMeshData()->AddLine(i0 + 3, i0 + 0);
	}
	
	void ShapeBatch::draw_line(float x1, float y1, float x2, float y2)
	{
		CHECK_BEGAN()
		CHECK_FLUSH_PRIM(Primitive::LINE, 2, 2)
	
		Vertex1P1C v1(x1, y1, 0.0f, color.r, color.g, color.b, color.a);
		Vertex1P1C v2(x2, y2, 0.0f, color.r, color.g, color.b, color.a);
	
		ADD_LINE(v1, v2);
	}

	void ShapeBatch::DrawFilledCircle(float X, float Y, float Radius, uint32 Segments)
	{
		DrawArc(X, Y, Radius, 0, 2 * PI, Segments);
	}

	void ShapeBatch::DrawDonut(float X, float Y, float InnerRadius, float OuterRadius, uint32 Segments)
	{
		
	}

	void ShapeBatch::DrawSubArc(float X, float Y, float InnerRadius, float OuterRadius, float Theta0, float Theta1, int32 Segments)
	{
		CHECK_BEGAN()
		//CHECK_FLUSH_PRIM(Primitive::TRIANGLE, Segments + 1, 3 * Segments);

	
	}

	void ShapeBatch::DrawArc()
	{
		CHECK_BEGAN()
		CHECK_FLUSH_PRIM(Primitive::TRIANGLE, Segments + 1, 3 * Segments);
	
	
	}

	void ShapeBatch::DrawStyledBox(float X, float Y, float W, float H, const Color& BackgroundColor, const Color& BorderColor, int32 BorderRadius, int32 BorderSize)
	{

	}D
	
	void ShapeBatch::flush()
	{
		if (mesh->GetMeshData()->GetVertexCount() > 0)
		{
			shader->uniformMat44("view_proj", proj * view);
			shader->uniformMat44("model", id4());

			mesh->Update();
			mesh->Render(prim);
			mesh->Reset();
		}
	}
	
	void ShapeBatch::end()
	{
		flush();
		began = false;
	}
	
	void ShapeBatch::set_color(const Ry::Vector4& color)
	{
		this->color = color;
	}
	
	void ShapeBatch::set_projection(const Matrix4& proj)
	{
		this->proj = proj;
	}

	void ShapeBatch::ResizeProjection(int32 Width, int32 Height)
	{
		proj = Ry::ortho4(0, Ry::app->GetViewportWidth(), Ry::app->GetViewportHeight(), 0, -1, 1);
	}
	
	void ShapeBatch::set_view(const Matrix4& view)
	{
		this->view = view;
	}
	
	void ShapeBatch::update(const Camera* cam)
	{
		proj = cam->get_proj();
		view = cam->get_view();
	}
	
	TextureBatch::TextureBatch()
	{
		shader = GetShader("Texture2D");
		mesh = new Mesh(VF1P1UV1C, BufferHint::DYNAMIC);
		color = Ry::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		mesh->GetMeshData()->SetShader(0, shader);
		began = false;
	
		view = id4();
		proj = Ry::ortho4(0, Ry::app->GetViewportWidth(), 0, Ry::app->GetViewportHeight(), -1, 1);
		texture = nullptr;
	}
	
	TextureBatch::~TextureBatch()
	{
		delete shader;
		delete mesh;
	}
	
	void TextureBatch::begin()
	{
		CHECK_ENDED()
	
		this->began = true;
	}
	
	void TextureBatch::draw_texture(Texture* texture, float x, float y, float w, float h)
	{
		CHECK_BEGAN()
		CHECK_FLUSH_TEXTURE(texture)

		float hw = w / 2;
		float hh = h / 2;
	
		Vertex1P1UV1C v1(x + -hw, y + -hh, 0.0f, 0.0f, 1.0f, color.r, color.g, color.b, color.a);
		Vertex1P1UV1C v2(x + -hw, y + hh, 0.0f, 0.0f, 0.0f, color.r, color.g, color.b, color.a);
		Vertex1P1UV1C v3(x + hw, y + hh, 0.0f, 1.0f, 0.0f, color.r, color.g, color.b, color.a);
		Vertex1P1UV1C v4(x + hw, y + -hh, 0.0f, 1.0f, 1.0f, color.r, color.g, color.b, color.a);
	
		ADD_QUAD(mesh, v1, v2, v3, v4);
	}

	void TextureBatch::draw_texture(Texture* texture, float x, float y, float u, float v, float uvw, float uvh, float ox, float oy, float w, float h)
	{
		CHECK_BEGAN()
		CHECK_FLUSH_TEXTURE(texture)

		// Top left
		Vertex1P1UV1C v1(0 - ox + x, 0 - oy + y, 0.0f, u, v, color.r, color.g, color.b, color.a);

		// Top right
		Vertex1P1UV1C v2(w - ox + x, 0 - oy + y, 0.0f, u + uvw, v, color.r, color.g, color.b, color.a);

		// Bottom right
		Vertex1P1UV1C v3(w - ox + x, -h - oy + y, 0.0f, u + uvw, v + uvh, color.r, color.g, color.b, color.a);

		// Bottom left
		Vertex1P1UV1C v4(0 - ox + x, -h - oy + y, 0.0f, u, v + uvh, color.r, color.g, color.b, color.a);

		ADD_QUAD(mesh, v1, v2, v3, v4);

	}
	
	void TextureBatch::flush()
	{
		if (mesh->GetMeshData()->GetVertexCount() > 0)
		{
			texture->Bind();
			{
				shader->uniformMat44("view_proj", proj * view);
				shader->uniformMat44("model", id4());
	
				// Update the mesh data for the new frame
				mesh->Update();

				// Render the new frame
				mesh->Render(Primitive::TRIANGLE);
				
				// Clear the data for this frame in preparation for next frame
				mesh->Reset();
			}
		}
	}
	
	void TextureBatch::end()
	{
		flush();
	
		began = false;
	}
	
	void TextureBatch::set_shader(Shader* shader)
	{
		this->shader = shader;
	}
	
	void TextureBatch::set_color(const Ry::Vector4& color)
	{
		this->color = color;
	}
	
	void TextureBatch::set_projection(const Matrix4& proj)
	{
		this->proj = proj;
	}

	void TextureBatch::ResizeProjection(int32 Width, int32 Height)
	{
		proj = Ry::ortho4(0, Ry::app->GetViewportWidth(), Ry::app->GetViewportHeight(), 0, -1, 1);
	}
	
	void TextureBatch::set_view(const Matrix4& view)
	{
		this->view = view;
	}
	
	void TextureBatch::update(const Camera* cam)
	{
		proj = cam->get_proj();
		view = cam->get_view();
	}*/

	TextBatch::TextBatch(Ry::SwapChain* Target, Shader2* Shad)
	{
		//this->FontShader = Ry::RenderAPI->make_shader(VF1P1UV, TEXT_VERT, TEXT_FRAG);

		//this->FontMesh = new Mesh(VF1P1UV, BufferHint::DYNAMIC);
		//this->FontMesh->GetMeshData()->SetShaderAll(FontShader);

		this->began = false;

		this->Font = nullptr;
		this->Color = Ry::Vector3(1.0f, 1.0f, 1.0f);

		CreateResources(Target);

		// Initialize pipeline
		CreatePipeline(VF1P1UV1C, Target, Shad ? Shad : GetShader("Font"));

		// Create command buffer
		CommandBuffer = Ry::NewRenderAPI->CreateCommandBuffer(Target);
		
	}

	void TextBatch::Begin()
	{
		CHECK_ENDED()
		this->began = true;
	}

	void TextBatch::RenderTextBodyWrapped(const Ry::Text& Text, float XPosition, float YPosition, float LineWidth)
	{

	}

	void TextBatch::End()
	{
		CHECK_BEGAN()

		Flush();

		this->began = false;
	}

	void TextBatch::Flush()
	{
		if (FontMesh->GetMeshData()->GetVertexCount() > 0)
		{

			
			// todo: change where viewport width and height are accessed at
			//FontShader->uniform_int32("HalfScreenWidth", Ry::GetViewportWidth() / 2);
			//FontShader->uniform_int32("HalfScreenHeight", Ry::GetViewportHeight() / 2);
			//FontShader->uniform_vec3("FontColor", Color);

			//Font->GetAtlasTexture()->Bind();
			{
				// Update the mesh data for the new frame
				FontMesh->Update();

				// Render the new frame
				// todo: mesh doesn't render directly
				//FontMesh->Render(Primitive::TRIANGLE);

				CommandBuffer->Submit();
				
				// Clear the data for this frame in preparation for next frame
				FontMesh->Reset();
			}
		}
	}
	
	void TextBatch::SetFont(Ry::BitmapFont* Font)
	{
		if(this->Font != Font && this->Font != nullptr)
		{
			Flush();
		}
		
		this->Font = Font;
	}

	void TextBatch::SetColor(const Ry::Vector3& Color)
	{
		this->Color = Color;
	}

	void TextBatch::CreateResources(SwapChain* Swap)
	{
		SceneResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ ShaderStage::Vertex }, 0);
		SceneResDesc->AddConstantBuffer(0, "Scene", {
			DeclPrimitive(Float4x4, "ViewProj")
			});
		SceneResDesc->CreateDescription();

		TextureResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ ShaderStage::Vertex }, 1);
		TextureResDesc->AddTextureBinding(0, "FontTexture");
		TextureResDesc->CreateDescription();

		// Create actual resource set now
		SceneRes = Ry::NewRenderAPI->CreateResourceSet(SceneResDesc, Swap);
		SceneRes->CreateBuffer();

		// Create texture resources
		TextureRes = Ry::NewRenderAPI->CreateResourceSet(TextureResDesc, Swap);
		TextureRes->CreateBuffer();

		Resources[0] = SceneRes;
		Resources[1] = TextureRes;
	}

	void TextBatch::CreatePipeline(const VertexFormat& Format, Ry::SwapChain* SwapChain, Ry::Shader2* Shad)
	{
		// Create new pipeline
		Ry::PipelineCreateInfo CreateInfo;
		CreateInfo.ViewportWidth = SwapChain->GetSwapChainWidth();
		CreateInfo.ViewportHeight = SwapChain->GetSwapChainHeight();
		CreateInfo.PipelineShader = Shad;
		CreateInfo.VertFormat = Format;
		CreateInfo.RenderPass = SwapChain->GetDefaultRenderPass();

		// Add resource description to pipeline
		CreateInfo.ResourceDescriptions.Add(SceneResDesc);

		Pipeline = Ry::NewRenderAPI->CreatePipeline(CreateInfo);
		Pipeline->CreatePipeline();
	}

	void TextBatch::RecordCommands()
	{
		CommandBuffer->Reset();

		CommandBuffer->BeginCmd();
		{
			CommandBuffer->BeginRenderPass();
			{
				CommandBuffer->SetViewportSize(0, 0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight());
				CommandBuffer->SetScissorSize(0, 0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight());

				// Bind pipeline
				CommandBuffer->BindPipeline(Pipeline);

				// Bind resources
				CommandBuffer->BindResources(Resources, 2);

				// Draw the mesh
				CommandBuffer->DrawVertexArrayIndexed(FontMesh->GetVertexArray(), FontMesh->GetMeshData()->Sections.get(0)->StartIndex, FontMesh->GetMeshData()->Sections.get(0)->Count);

			}
			CommandBuffer->EndRenderPass();

		}
		CommandBuffer->EndCmd();
	}
	
}

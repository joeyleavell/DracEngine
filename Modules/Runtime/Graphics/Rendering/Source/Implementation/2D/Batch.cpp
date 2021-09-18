#include "2D/Batch/Batch.h"
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
#include "Interface/Pipeline.h"
#include "SwapChain.h"
#include "Interface/RenderAPI.h"
#include "Interface/RenderingResource.h"
#include "Interface/RenderCommand.h"
#include "Interface/VertexArray.h"
#include "Interface/Texture.h"

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

	void BatchHollowRectangle(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, float X, float Y, float Width, float Height, float Thickness, float Depth)
	{
		// Clear the previous item data
		Item->Clear();

		// Left border
		Item->AddVertex1P1C(X - Thickness, Y - Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X - Thickness, Y + Height + Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X, Y + Height + Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X, Y - Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

		// Top border
		Item->AddVertex1P1C(X - Thickness, Y + Height, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X - Thickness, Y + Height + Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width + Thickness, Y + Height + Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width + Thickness, Y + Height, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

		// Right border
		Item->AddVertex1P1C(X + Width, Y - Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width, Y + Height + Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width + Thickness, Y + Height + Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width + Thickness, Y - Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

		// Bottom border
		Item->AddVertex1P1C(X - Thickness, Y - Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X - Thickness, Y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width + Thickness, Y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C(X + Width + Thickness, Y - Thickness, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

		// Add rects
		for(int32 Rect = 0; Rect < 4; Rect++)
		{
			int32 Base = Rect * 4;
			Item->AddTriangle(Base + 0, Base + 1, Base + 2);
			Item->AddTriangle(Base + 2, Base + 3, Base + 0);
		}
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

		float OriginXModel = OriginX * Width;
		float OriginYModel = OriginY * Height;

		Item->AddVertex1P1UV1C(0 - OriginXModel + X, 0 + OriginYModel + Y, Depth, U, V, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C(Width - OriginXModel + X, 0 + OriginYModel + Y, Depth, U + UVWidth, V, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C(Width - OriginXModel + X, -Height + OriginYModel + Y, Depth, U + UVWidth, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C(0 - OriginXModel + X, -Height + OriginYModel + Y, Depth, U, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);

		Item->AddTriangle(0, 1, 2);
		Item->AddTriangle(2, 3, 0);
	}

	void BatchText(Ry::SharedPtr<BatchItemSet> ItemSet, const Ry::Color& Color, BitmapFont* Font, const PrecomputedTextData& TextData, float XPosition, float YPosition, float LineWidth)
	{
		ItemSet->Items.Clear();

		// The amount of spaces in a tab
		const int32 TAB_SPACES = 4;
		const int32 SPACE_ADVANCE = Font->GetGlyph(static_cast<int32>(' '))->AdvanceWidth;

		// Establish origin
		float CurX = XPosition;
		float CurY = YPosition + (Font->GetAscent());

		// Get the advance width of the space character

		for (int32 Line = 0; Line < TextData.Lines.GetSize(); Line++)
		{
			int32 WordCount = TextData.Lines[Line].Words.GetSize();
			for (int32 Word = 0; Word < WordCount; Word++)
			{
				const Ry::String& CurrentWord = TextData.Lines[Line].Words[Word];
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
	
	Batch::Batch(Ry::SwapChain* Target, Ry::RenderPass* ParentPass)
	{
		this->ParentPass = ParentPass;		
		this->Swap = Target;
		
		View = Ry::id4();
		Projection = Ry::ortho4(0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight(), 0.0f, -1.0f, 1.0f);

		// Add default pipelines
		AddPipeline("Shape", "Shape");
		AddPipeline("Texture", "Texture");
		AddPipeline("Font", "Font");

	}

	void Batch::AddPipeline(Ry::String Name, Ry::String Shader)
	{
		Ry::Shader* ShaderToUse = ShaderToUse = Ry::GetShader(Shader);

		Ry::BatchPipeline* NewPipeline = new Ry::BatchPipeline;

		NewPipeline->SceneResDesc   = ShaderToUse->GetVertexReflectionData()[0];
		NewPipeline->TextureResDesc = ShaderToUse->GetFragmentReflectionData()[1];
		NewPipeline->Format         = ShaderToUse->GetVertexFormat();

		// Create new pipeline
		Ry::PipelineCreateInfo CreateInfo;
		CreateInfo.ViewportWidth = Swap->GetSwapChainWidth();
		CreateInfo.ViewportHeight = Swap->GetSwapChainHeight();
		CreateInfo.PipelineShader = ShaderToUse;
		CreateInfo.RenderPass = Swap->GetDefaultRenderPass();
		CreateInfo.Depth.bEnableDepthTest = false; // Turn off depth test
		CreateInfo.Blend.bEnabled = true; // Enable blending

		NewPipeline->Pipeline = Ry::RendAPI->CreatePipelineFromShader(CreateInfo, ShaderToUse);
		NewPipeline->Pipeline->CreatePipeline();

		// Create scene resources for this pipeline
		NewPipeline->SceneResources = Ry::RendAPI->CreateResourceSet(NewPipeline->SceneResDesc, Swap);
		NewPipeline->SceneResources->CreateBuffer();

		Pipelines.insert(Name, NewPipeline);
	}

	void Batch::AddItem(Ry::SharedPtr<BatchItem> Item, Ry::String PipelineId, RectScissor Scissor, Texture* Text, int32 Layer)
	{
		if (Layer < 0)
			Layer = Layers.GetSize() + 1;

		BatchGroup* Group = FindOrCreateBatchGroup(PipelineId, Scissor, Text, Layer);

		if(Group)
		{
			Group->Items.Insert(Item);

			Layers[Layer]->bNeedsRecord = true;
		}

	}

	void Batch::AddItemSet(Ry::SharedPtr<BatchItemSet> Item, Ry::String PipelineId, RectScissor Scissor, Texture* Text, int32 Layer)
	{
		if (Layer < 0)
			Layer = Layers.GetSize() + 1;

		BatchGroup* Group = FindOrCreateBatchGroup(PipelineId, Scissor, Text, Layer);
		if(Group)
		{
			Group->ItemSets.Insert(Item);

			Layers[Layer]->bNeedsRecord = true;
		}

	}

	void Batch::RemoveItem(Ry::SharedPtr<BatchItem> Item)
	{
		// Try remove item from all batch groups (todo: make more efficient, probably by using hash sets)

		for(BatchLayer* Layer : Layers)
		{
			Ry::KeyIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreateKeyIterator();
			while(PipelineItr)
			{
				for (BatchGroup* Group : *PipelineItr.Value())
				{
					Group->Items.Remove(Item);
				}

				++PipelineItr;
			}


			Layer->bNeedsRecord = true;
		}

	}

	void Batch::RemoveItemSet(Ry::SharedPtr<BatchItemSet> Item)
	{
		for (BatchLayer* Layer : Layers)
		{
			Ry::KeyIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreateKeyIterator();
			while (PipelineItr)
			{
				for (BatchGroup* Group : *PipelineItr.Value())
				{
					Group->ItemSets.Remove(Item);
				}
				
				++PipelineItr;
			}

			Layer->bNeedsRecord = true;
		}

	}

	void Batch::Clear()
	{
		for (BatchLayer* Layer : Layers)
			delete Layer;
		
		Layers.Clear();
	}

	void Batch::SetView(const Matrix4& View)
	{
		this->View = View;
	}

	void Batch::Resize(int32 Width, int32 Height)
	{
		Projection = Ry::ortho4(0, (float) Width, (float)Height, 0, -1, 1);

		// Mark all layers as needing recording
		for(BatchLayer* Layer : Layers)
		{
			Layer->bNeedsRecord = true;
		}
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
		auto AddItem = [this](Ry::SharedPtr<BatchItem> Item, Mesh* BatchMesh)
		{
			uint32 BaseIndex = BatchMesh->GetMeshData()->GetVertexCount();

			// Add all vertices and indices
			BatchMesh->GetMeshData()->AddVertexRaw(Item->Data.GetData(), (int32)Item->Data.GetSize(), Item->VertexCount);
			BatchMesh->GetMeshData()->AddIndexRaw(Item->Indices.GetData(), (int32)Item->Indices.GetSize(), BaseIndex);
		};

		// Reset/create meshes
		for (BatchLayer* Layer : Layers)
		{
			Ry::KeyIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreateKeyIterator();
			while (PipelineItr)
			{
				for (BatchGroup* Group : *PipelineItr.Value())
				{
					if (Group->BatchMesh)
					{
						Group->BatchMesh->Reset();
					}

					// Rebuild the mesh
					for (SharedPtr<BatchItem> Item : Group->Items)
					{
						AddItem(Item, Group->BatchMesh);
					}

					for (SharedPtr<BatchItemSet> ItemSet : Group->ItemSets)
					{
						for (SharedPtr<BatchItem> Item : ItemSet->Items)
						{
							AddItem(Item, Group->BatchMesh);
						}
					}

					Group->BatchMesh->Update();

					// See if the amount of indices in this mesh has changed
					int CurrentIndexCount = Group->BatchMesh->GetVertexArray()->GetIndexCount();
					if (Group->LastIndexCount < 0 || Group->LastIndexCount != CurrentIndexCount)
					{
						Layer->bNeedsRecord = true;
						Group->LastIndexCount = CurrentIndexCount;
					}
				}

				++PipelineItr;
			}
		}

	}

	bool Batch::Render()
	{
		bool bReturn = false;
		
		// Update resource resets and re-record command buffers if needed
		for (BatchLayer* Layer : Layers)
		{
			Ry::KeyIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreateKeyIterator();
			while (PipelineItr)
			{
				BatchPipeline* Key = *PipelineItr.Key();

				// Update uniform
				// Todo: change this to a push constant and only change it if needed
				Key->SceneResources->SetMatConstant("Scene", "ViewProjection", Projection * View);

				for (BatchGroup* Group : *PipelineItr.Value())
				{
					for (ResourceSet* Set : Group->ResourceSets)
					{
						if (Set->Update())
						{
							Layer->bNeedsRecord = true;
						}
					}
				}

				++PipelineItr;
			}

			bool bWasDirty = false;

			if(Layer->CommandBuffer)
			{
				bWasDirty = Layer->CommandBuffer->CheckDirty();
			}

			// Return true if at least one of the command buffers needs to be recorded
			// This return value indicates that the primary command buffer needs to be re-recorded
			bReturn |= bWasDirty || Layer->bNeedsRecord;

			// Check dirty will see if the command buffer needs to be re-recorded
			if (Layer->bNeedsRecord)
			{
				if (!bWasDirty)
				{
					// Re-record commands
					RecordCommands(Layer->Depth);
				}

				Layer->bNeedsRecord = false;
			}
			
		}

		return bReturn;
	}

	int32 Batch::GetLayerCount() const
	{
		return Layers.GetSize();
	}

	void Batch::SetRenderPass(RenderPass* ParentRenderPass)
	{
		this->ParentPass = ParentRenderPass;
		for (BatchLayer* Layer : Layers)
		{
			Layer->CommandBuffer->UpdateParentRenderPass(ParentRenderPass);
		}
	}

	Ry::CommandBuffer* Batch::GetCommandBuffer(int32 Layer)
	{
		if(Layer < Layers.GetSize())
		{
			BatchLayer* BatLayer = Layers[Layer];
			return BatLayer->CommandBuffer;
		}

		return nullptr;
	}

	BatchGroup* Batch::FindOrCreateBatchGroup(Ry::String PipelineId, RectScissor Scissor, Texture* Text, int32 Layer)
	{
		BatchGroup* Existing = FindBatchGroup(Text, Scissor, Layer);

		if(!Existing)
		{
			// Only creates if needed
			CreateLayersIfNeeded(Layer);

			BatchLayer* AtLayer = Layers[Layer];
			
			BatchGroup* NewGroup = new BatchGroup;

			NewGroup->OwningPipeline = (*Pipelines.get(PipelineId));
			NewGroup->Text = Text;			
			NewGroup->Scissor = Scissor;
			NewGroup->ResourceSets.Add(NewGroup->OwningPipeline->SceneResources);

			// Create a new texture resource if needed
			if(Text)
			{
				NewGroup->TextureResources = Ry::RendAPI->CreateResourceSet(NewGroup->OwningPipeline->TextureResDesc, AtLayer->CommandBuffer->GetSwapChain());
				NewGroup->TextureResources->BindTexture("BatchTexture", Text);
				NewGroup->TextureResources->CreateBuffer();

				NewGroup->ResourceSets.Add(NewGroup->TextureResources);
			}
			else
			{
				NewGroup->TextureResources = nullptr;
			}

			// Create group mesh
			NewGroup->BatchMesh = new Mesh(NewGroup->OwningPipeline->Format);

			// Add this group to the layer, create array if needed
			if(AtLayer->Groups.contains(NewGroup->OwningPipeline))
			{
				(*AtLayer->Groups.get(NewGroup->OwningPipeline)).Add(NewGroup);
			}
			else
			{
				AtLayer->Groups.insert(NewGroup->OwningPipeline, Ry::ArrayList<BatchGroup*>{NewGroup});
			}

			return NewGroup;
		}
		else
		{
			return Existing;
		}

	}

	BatchGroup* Batch::FindBatchGroup(Texture* Text, RectScissor Scissor, int32 Layer)
	{
		if (Layer >= Layers.GetSize())
			return nullptr;

		BatchLayer* LayerPtr = Layers[Layer];
		Ry::KeyIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = LayerPtr->Groups.CreateKeyIterator();
		while (PipelineItr)
		{
			for (BatchGroup* Group : *PipelineItr.Value())
			{
				if (Group->Text == Text && Group->Scissor == Scissor)
				{
					return Group;
				}
			}
			++PipelineItr;
		}

		return nullptr;
	}

	void Batch::DeleteEmptyGroups()
	{
		Ry::ArrayList<BatchLayer*> LayersToRemove;

		for(BatchLayer* Layer : Layers)
		{

			Ry::KeyIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreateKeyIterator();
			while (PipelineItr)
			{
				Ry::ArrayList<BatchGroup*> GroupsToRemove;

				for (BatchGroup* Group : *PipelineItr.Value())
				{
					//if (Group->Items.GetSize() == 0 && Group->ItemSets.GetSize() == 0)
					{
						GroupsToRemove.Add(Group);
					}
				}

				for (BatchGroup* RemoveGroup : GroupsToRemove)
				{
					// Delete group batch mesh
					RemoveGroup->BatchMesh->DeleteMesh();
					delete RemoveGroup->BatchMesh;
					RemoveGroup->BatchMesh = nullptr;

					// Delete group resources
					if (RemoveGroup->TextureResources)
					{
						RemoveGroup->TextureResources->DeleteBuffer();
						delete RemoveGroup->TextureResources;
						RemoveGroup->TextureResources = nullptr;
					}

					(*Layer->Groups.get(*PipelineItr.Key())).Remove(RemoveGroup);

					delete RemoveGroup;
				}

				++PipelineItr;
			}
			
		}
	}

	void Batch::CreateLayersIfNeeded(int32 Index)
	{
		// Create layers as needed
		while (Index >= Layers.GetSize())
		{
			BatchLayer* NewLayer = new BatchLayer;

			// Init depth
			NewLayer->Depth = Layers.GetSize();
			NewLayer->CommandBuffer = Ry::RendAPI->CreateCommandBuffer(Swap, ParentPass);
			NewLayer->bNeedsRecord = true;

			Layers.Add(NewLayer);
			RecordCommands(Layers.GetSize() - 1);

		}
	}

	void Batch::RecordCommands(int32 Layer)
	{
		BatchLayer* AtLayer = Layers[Layer];
		
		AtLayer->CommandBuffer->Reset();
		
		AtLayer->CommandBuffer->BeginCmd();
		{
			Ry::KeyIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = AtLayer->Groups.CreateKeyIterator();
			while (PipelineItr)
			{
				BatchPipeline* Key = *PipelineItr.Key();

				// Bind pipeline
				AtLayer->CommandBuffer->BindPipeline(Key->Pipeline);

				AtLayer->CommandBuffer->SetViewportSize(0, 0, (float)Swap->GetSwapChainWidth(), (float)Swap->GetSwapChainHeight());

				for (const BatchGroup* Group : *PipelineItr.Value())
				{
					if(Group->Scissor.IsEnabled())
					{
						// Do conversion
						int32 ConvertedY = Swap->GetSwapChainHeight() - (Group->Scissor.Y + Group->Scissor.Height);
						AtLayer->CommandBuffer->SetScissorSize(Group->Scissor.X, ConvertedY, Group->Scissor.Width, Group->Scissor.Height);
					}
					else
					{
						AtLayer->CommandBuffer->SetScissorSize(0, 0, (float)Swap->GetSwapChainWidth(), (float)Swap->GetSwapChainHeight());
					}

					if (Group->Items.GetSize() > 0 || Group->ItemSets.GetSize() > 0)
					{
						// Bind resources
						AtLayer->CommandBuffer->BindResources(Group->ResourceSets.GetData(), Group->ResourceSets.GetSize());

						// Draw the mesh
						MeshSection& Section = *Group->BatchMesh->GetMeshData()->Sections.get(0);
						AtLayer->CommandBuffer->DrawVertexArrayIndexed(Group->BatchMesh->GetVertexArray(), Section.StartIndex, Section.Count);

					}
				}

				++PipelineItr;
			}

		}
		AtLayer->CommandBuffer->EndCmd();
	}

	TextBatch::TextBatch(Ry::SwapChain* Target, Shader* Shad)
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
		CommandBuffer = Ry::RendAPI->CreateCommandBuffer(Target);
		
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
		SceneResDesc = Ry::RendAPI->CreateResourceSetDescription({ ShaderStage::Vertex }, 0);
		SceneResDesc->AddConstantBuffer(0, "Scene", {
			DeclPrimitive(Float4x4, "ViewProj")
			});
		SceneResDesc->CreateDescription();

		TextureResDesc = Ry::RendAPI->CreateResourceSetDescription({ ShaderStage::Vertex }, 1);
		TextureResDesc->AddTextureBinding(0, "FontTexture");
		TextureResDesc->CreateDescription();

		// Create actual resource set now
		SceneRes = Ry::RendAPI->CreateResourceSet(SceneResDesc, Swap);
		SceneRes->CreateBuffer();

		// Create texture resources
		TextureRes = Ry::RendAPI->CreateResourceSet(TextureResDesc, Swap);
		TextureRes->CreateBuffer();

		Resources[0] = SceneRes;
		Resources[1] = TextureRes;
	}

	void TextBatch::CreatePipeline(const VertexFormat& Format, Ry::SwapChain* SwapChain, Ry::Shader* Shad)
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

		Pipeline = Ry::RendAPI->CreatePipeline(CreateInfo);
		Pipeline->CreatePipeline();
	}

	void TextBatch::RecordCommands()
	{
		CommandBuffer->Reset();

		CommandBuffer->BeginCmd();
		{
			//CommandBuffer->BeginRenderPass();
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
			//CommandBuffer->EndRenderPass();

		}
		CommandBuffer->EndCmd();
	}
	
}

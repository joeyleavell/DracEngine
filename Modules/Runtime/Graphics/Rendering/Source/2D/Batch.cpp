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
	void ComputeTextData(PrecomputedTextData& OutData, const Ry::String Text)
	{
		OutData.Lines.Clear();

		Ry::StringView* Lines = nullptr;
		int32 LineCount = Text.split("\n", &Lines);
		for (int32 Line = 0; Line < LineCount; Line++)
		{
			TextLine NewLine;

			Ry::StringView* Words = nullptr;
			int32 WordCount = Lines[Line].split(" ", &Words);
			for (int32 Word = 0; Word < WordCount; Word++)
			{
				NewLine.Words.Add(Words[Word]);
			}

			// Add the cached line
			OutData.Lines.Add(NewLine);

			delete[] Words;
		}
		delete[] Lines;

	}

	Ry::SharedPtr<BatchItem> MakeItem()
	{
		return Ry::MakeShared<>(new BatchItem);
	}

	Ry::SharedPtr<BatchItemSet> MakeItemSet()
	{
		return Ry::MakeShared<>(new BatchItemSet);
	}

	void TransformedRect(Ry::Vector3* OutVerts, Ry::Matrix3 Transform, float OriginX, float OriginY, float Width, float Height)
	{
		float OriginXModel = OriginX * Width;
		float OriginYModel = OriginY * Height;

		Ry::Vector3 V1{ 0.0f, 0.0f, 1.0f },
			V2{ 0.0f, Height, 1.0f },
			V3{ Width, Height, 1.0f },
			V4{ Width, 0.0f, 1.0f };

		V1.x -= OriginXModel;
		V1.y -= OriginYModel;
		V2.x -= OriginXModel;
		V2.y -= OriginYModel;
		V3.x -= OriginXModel;
		V3.y -= OriginYModel;
		V4.x -= OriginXModel;
		V4.y -= OriginYModel;

		OutVerts[0] = Transform * V1;
		OutVerts[1] = Transform * V2;
		OutVerts[2] = Transform * V3;
		OutVerts[3] = Transform * V4;

	}

	void BatchRectangleTransform(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Color, Ry::Matrix3 Transform,
	                             float Width, float Height, float OriginX, float OriginY, float Depth)
	{
		Ry::Vector3 Verts[4];
		TransformedRect(Verts, Transform, OriginX, OriginY, Width, Height);

		Item->AddVertex1P1C((float)Verts[0].x, (float)Verts[0].y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C((float)Verts[1].x, (float)Verts[1].y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C((float)Verts[2].x, (float)Verts[2].y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);
		Item->AddVertex1P1C((float)Verts[3].x, (float)Verts[3].y, Depth, Color.Red, Color.Green, Color.Blue, Color.Alpha);

		Item->AddTriangle(0, 1, 2);
		Item->AddTriangle(2, 3, 0);
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
		Item->AddVertex1P1UV1C(Width - OriginXModel + X, Height + OriginYModel + Y, Depth, U + UVWidth, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C(0 - OriginXModel + X, Height + OriginYModel + Y, Depth, U, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);

		Item->AddTriangle(0, 1, 2);
		Item->AddTriangle(2, 3, 0);
	}

	void BatchTextureTransform(Ry::SharedPtr<BatchItem> Item, const Ry::Color& Tint, Ry::Matrix3 Transform, float U,
		float V, float UVWidth, float UVHeight, float OriginX, float OriginY, float Width, float Height, float Depth)
	{
		Item->Clear();

		float OriginXModel = OriginX * Width;
		float OriginYModel = OriginY * Height;

		Ry::Vector3 V1 { 0.0f, 0.0f, 1.0f },
			V2{ 0.0f, Height, 1.0f },
			V3{ Width, Height, 1.0f },
		V4 {Width, 0.0f, 1.0f };

		V1.x -= OriginXModel;
		V1.y -= OriginYModel;
		V2.x -= OriginXModel;
		V2.y -= OriginYModel;
		V3.x -= OriginXModel;
		V3.y -= OriginYModel;
		V4.x -= OriginXModel;
		V4.y -= OriginYModel;

		V1 = Transform * V1;
		V2 = Transform * V2;
		V3 = Transform * V3;
		V4 = Transform * V4;

		Item->AddVertex1P1UV1C((float) V1.x, (float) V1.y, Depth, U, V, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C((float)V2.x, (float)V2.y, Depth, U, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C((float)V3.x, (float)V3.y, Depth, U + UVWidth, V + UVHeight, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);
		Item->AddVertex1P1UV1C((float)V4.x, (float)V4.y, Depth, U + UVWidth, V, Tint.Red, Tint.Green, Tint.Blue, Tint.Alpha);

		Item->AddTriangle(0, 1, 2);
		Item->AddTriangle(2, 3, 0);
	}

	void BatchText(Ry::SharedPtr<BatchItemSet> ItemSet, const Ry::Color& Color, BitmapFont* Font, const PrecomputedTextData& TextData, float XPosition, float YPosition, float LineWidth)
	{
		
		// The amount of spaces in a tab
		const int32 TAB_SPACES = 4;
		const int32 SPACE_ADVANCE = Font->GetGlyph(static_cast<int32>(' '))->AdvanceWidth;

		// Establish origin
		float CurX = XPosition;
		float CurY = YPosition - Font->GetAscent();

		// Get the advance width of the space character

		int32 GlyphIndex = 0;
		
		for (int32 Line = 0; Line < TextData.Lines.GetSize(); Line++)
		{
			int32 WordCount = TextData.Lines[Line].Words.GetSize();
			for (int32 Word = 0; Word < WordCount; Word++)
			{
				const Ry::String& CurrentWord = TextData.Lines[Line].Words[Word];
				float TextWidth = Font->MeasureWidth(CurrentWord);

				// Wrap text to next line
				if (Word > 0 && CurX + TextWidth > XPosition + LineWidth)
				{
					// Reset cursor
					CurX = XPosition;
					CurY += -(Font->GetAscent() - Font->GetDescent() + Font->GetLineGap());
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

						float BL_X = CurX + OriginX;
						float BL_Y = CurY - OriginY;

						float BR_X = BL_X + RasterWidth;
						float BR_Y = BL_Y;

						float TL_X = BL_X;
						float TL_Y = BL_Y + RasterHeight;

						float TR_X = BL_X + RasterWidth;
						float TR_Y = BL_Y + RasterHeight;

						// Create a batch item for this glyph
						Ry::SharedPtr<BatchItem> GlyphItem;
						if(GlyphIndex < ItemSet->Items.GetSize())
						{
							GlyphItem = ItemSet->Items[GlyphIndex];
							GlyphItem->Clear();
						}
						else
						{
							GlyphItem = MakeItem();
							ItemSet->AddItem(GlyphItem);
						}
						
						GlyphIndex++;

						// Transform vert
						GlyphItem->AddVertex1P1UV1C(TL_X, TL_Y, 0.0f, U, V, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Top left
						GlyphItem->AddVertex1P1UV1C(TR_X, TR_Y, 0.0f, U + UVW, V, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Top right
						GlyphItem->AddVertex1P1UV1C(BR_X, BR_Y, 0.0f, U + UVW, V + UVH, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Bottom right
						GlyphItem->AddVertex1P1UV1C(BL_X, BL_Y, 0.0f, U, V + UVH, Color.Red, Color.Green, Color.Blue, Color.Alpha); // Bottom left
						GlyphItem->AddTriangle(2, 1, 0);
						GlyphItem->AddTriangle(0, 3, 2);

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
			CurY += -(Font->GetAscent() - Font->GetDescent() + Font->GetLineGap());
			CurX = XPosition;
		}

		// Remove extraneous glyphs
		int32 Count = ItemSet->Items.GetSize() - GlyphIndex;
		for(int32 RemoveIndex = 0; RemoveIndex < Count; RemoveIndex++)
		{
			ItemSet->Items.RemoveAt(ItemSet->Items.GetSize() - 1);
		}
		
	}

	Batch::Batch(Ry::SwapChain* Target, Ry::RenderPass* ParentPass)
	{
		this->ParentPass = ParentPass;		
		this->Swap = Target;
		
		View = Ry::id4();
		Projection = Ry::ortho4(0, (float)Ry::GetViewportWidth(), 0.0f, (float)Ry::GetViewportHeight(), -1.0f, 1.0f);

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

	void Batch::AddItem(Ry::SharedPtr<BatchItem> Item, Ry::String PipelineId, PipelineState State, Texture* Text, int32 Layer)
	{
		if (Layer < 0)
			Layer = Layers.GetSize() - 1;

		BatchGroup* Group = FindOrCreateBatchGroup(PipelineId, State, Text, Layer);

		if(Group)
		{
			Group->Items.Insert(Item);

			Layers[Layer]->bNeedsRecord = true;
		}

	}

	void Batch::AddItemSet(Ry::SharedPtr<BatchItemSet> Item, Ry::String PipelineId, PipelineState Scissor, Texture* Text, int32 Layer)
	{
		if (Layer < 0)
			Layer = Layers.GetSize() - 1;

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
			Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreatePairIterator();
			while(PipelineItr)
			{
				for (BatchGroup* Group : PipelineItr.GetValue())
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
			Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreatePairIterator();
			while (PipelineItr)
			{
				for (BatchGroup* Group : PipelineItr.GetValue())
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
		{
			Ry::OAPairIterator<BatchPipeline*, Ry::ArrayList<BatchGroup*>> Itr = Layer->Groups.CreatePairIterator();
			while(Itr)
			{
				for(int32 GroupIndex = 0; GroupIndex < Itr.GetValue().GetSize(); GroupIndex++)
				{
					BatchGroup* Group = Itr.GetValue()[GroupIndex];

					Group->Items.Clear();
					Group->ItemSets.Clear();
				}

				++Itr;
			}
		}

		Update();		
	}

	void Batch::SetView(const Matrix4& View)
	{
		this->View = View;
	}

	void Batch::Resize(int32 Width, int32 Height)
	{
		Projection = Ry::ortho4(0.0f, (float) Width, (float)0.0f, (float) Height, -1.0f, 1.0f);

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
		this->View = Cam->GetView();
		this->Projection = Cam->GetProjection();
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

		int32 Items = 0;

		// Reset/create meshes
		for (BatchLayer* Layer : Layers)
		{
			Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreatePairIterator();
			while (PipelineItr)
			{
				for (BatchGroup* Group : PipelineItr.GetValue())
				{
					if (Group->BatchMesh)
					{
						Group->BatchMesh->Reset();
					}

					Items += Group->Items.GetSize();

					// Rebuild the mesh
					OASetIterator<SharedPtr<BatchItem>> ItemItr = Group->Items.CreatePairIterator();
					while(ItemItr)
					{
						AddItem(*ItemItr, Group->BatchMesh);
						++ItemItr;
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
			Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreatePairIterator();
			while (PipelineItr)
			{
				BatchPipeline* Key = PipelineItr.GetKey();

				// Update uniform
				// Todo: change this to a push constant and only change it if needed
				Key->SceneResources->SetMatConstant("Scene", "ViewProjection", Projection * View);

				for (BatchGroup* Group : PipelineItr.GetValue())
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

	void Batch::UpdatePipelineState(const PipelineState& State)
	{
		for (BatchLayer* Layer : Layers)
		{
			Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreatePairIterator();
			while (PipelineItr)
			{
				for (BatchGroup* Group : PipelineItr.GetValue())
				{
					if (Group->State == State)
					{
 						Group->State = State;
					}
				}
				++PipelineItr;
			}
			Layer->bNeedsRecord = true;
		}
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

	BatchGroup* Batch::FindOrCreateBatchGroup(Ry::String PipelineId, PipelineState State, Texture* Text, int32 Layer)
	{
		BatchGroup* Existing = FindBatchGroup(Text, State, Layer);

		if(!Existing)
		{
			// Only creates if needed
			CreateLayersIfNeeded(Layer);

			BatchLayer* AtLayer = Layers[Layer];
			
			BatchGroup* NewGroup = new BatchGroup;

			NewGroup->OwningPipeline = (*Pipelines.get(PipelineId));
			NewGroup->Text = Text;			
			NewGroup->State = State;
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
			if(AtLayer->Groups.Contains(NewGroup->OwningPipeline))
			{
				AtLayer->Groups.Get(NewGroup->OwningPipeline).Add(NewGroup);
			}
			else
			{
				AtLayer->Groups.Insert(NewGroup->OwningPipeline, Ry::ArrayList<BatchGroup*>{NewGroup});
			}

			return NewGroup;
		}
		else
		{
			return Existing;
		}

	}

	BatchGroup* Batch::FindBatchGroup(Texture* Text, PipelineState State, int32 Layer)
	{
		if (Layer >= Layers.GetSize())
			return nullptr;

		BatchLayer* LayerPtr = Layers[Layer];
		Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = LayerPtr->Groups.CreatePairIterator();
		while (PipelineItr)
		{
			for (BatchGroup* Group : PipelineItr.GetValue())
			{
				if (Group->Text == Text && Group->State == State)
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

			Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = Layer->Groups.CreatePairIterator();
			while (PipelineItr)
			{
				Ry::ArrayList<BatchGroup*> GroupsToRemove;

				for (BatchGroup* Group : PipelineItr.GetValue())
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

					Layer->Groups.Get(PipelineItr.GetKey()).Remove(RemoveGroup);

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
			Ry::OAPairIterator<Ry::BatchPipeline*, Ry::ArrayList<BatchGroup*>> PipelineItr = AtLayer->Groups.CreatePairIterator();
			while (PipelineItr)
			{
				BatchPipeline* Key = PipelineItr.GetKey();

				// Bind pipeline
				AtLayer->CommandBuffer->BindPipeline(Key->Pipeline);

				AtLayer->CommandBuffer->SetViewportSize(0, 0, (int32) Swap->GetSwapChainWidth(), (int32)Swap->GetSwapChainHeight());

				for (const BatchGroup* Group : PipelineItr.GetValue())
				{
					RectScissor Scissor = Group->State.Scissor;
					if(Scissor.IsEnabled())
					{
						float Px = Scissor.X / ((float) Ry::GetViewportWidth());
						float Py = Scissor.Y / ((float) Ry::GetViewportHeight());
						float Pw = Scissor.Width / ((float) Ry::GetViewportWidth());
						float Ph = Scissor.Height / ((float) Ry::GetViewportHeight());

						AtLayer->CommandBuffer->SetScissorSize(
							Px * Swap->GetSwapChainWidth(), 
							Py * Swap->GetSwapChainHeight(), 
							Pw * Swap->GetSwapChainWidth(), 
							Ph * Swap->GetSwapChainHeight()
						);
					}
					else
					{
						AtLayer->CommandBuffer->SetScissorSize(0, 0, (int32)Swap->GetSwapChainWidth(), (int32)Swap->GetSwapChainHeight());
					}

					if (!Group->Items.IsEmpty() || !Group->ItemSets.IsEmpty())
					{
						// Bind resources
						AtLayer->CommandBuffer->BindResources(Group->ResourceSets.GetData(), Group->ResourceSets.GetSize());

						// Draw the mesh
						MeshSection& Section = Group->BatchMesh->GetMeshData()->Sections.Get(0);

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
				CommandBuffer->SetViewportSize(0, 0, (int32)Ry::GetViewportWidth(), (int32)Ry::GetViewportHeight());
				CommandBuffer->SetScissorSize(0, 0, (int32)Ry::GetViewportWidth(), (int32)Ry::GetViewportHeight());

				// Bind pipeline
				CommandBuffer->BindPipeline(Pipeline);

				// Bind resources
				CommandBuffer->BindResources(Resources, 2);

				// Draw the mesh
				CommandBuffer->DrawVertexArrayIndexed(FontMesh->GetVertexArray(), FontMesh->GetMeshData()->Sections.Get(0).StartIndex, FontMesh->GetMeshData()->Sections.Get(0).Count);

			}
			//CommandBuffer->EndRenderPass();

		}
		CommandBuffer->EndCmd();
	}
	
}

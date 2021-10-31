#include "Tile.h"
#include "World2D.h"
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_world.h>
#include "box2d/b2_chain_shape.h"

namespace Ry
{
	TileSheet::TileSheet(Texture* Tiles, int32 TileWidth, int32 TileHeight)
	{
		this->Tilesheet = Tiles;
		this->TileWidth = TileWidth;
		this->TileHeight = TileHeight;
	}

	void TileSheet::RegisterTile(int32 TileID, int32 TilesheetX, int32 TilesheetY)
	{
		TextureRegion NewRegion;
		NewRegion.X = TilesheetX * TileWidth;
		NewRegion.Y = TilesheetY * TileHeight;
		NewRegion.W = TileWidth;
		NewRegion.H = TileHeight;
		NewRegion.Parent = Tilesheet;

		Tiles.Insert(TileID, NewRegion);
	}

	TextureRegion TileSheet::GetTile(int32 TileID)
	{
		return Tiles.Get(TileID);
	}

	Texture* TileSheet::GetTileSheetTexture() const
	{
		return Tilesheet;
	}

	int32 TileSheet::GetTileWidth() const
	{
		return TileWidth;
	}

	int32 TileSheet::GetTileHeight() const
	{
		return TileHeight;
	}

	TileLayer::TileLayer(int32 TilesWide, int32 TilesTall)
	{
		Tiles = new TileCell[TilesWide * TilesTall];
		this->Width = TilesWide;
		this->Height = TilesTall;

		memset(Tiles, 0, TilesWide * TilesTall * sizeof(TileCell));

		for(int32 X = 0; X < Width; X++)
			for(int32 Y = 0; Y < Height; Y++)
			{
				Tiles[X + Y * Width].X = X;
				Tiles[X + Y * Width].Y = Y;
			}

	}

	TileLayer::~TileLayer()
	{
		delete[] Tiles;
	}

	void TileLayer::SetTile(int32 X, int32 Y, int32 ID)
	{
		Tiles[X + Y * Width].TileID = ID;
	}

	int32 TileLayer::GetTile(int32 X, int32 Y) const
	{
		return Tiles[X + Y * Width].TileID;
	}

	TileCell* TileLayer::GetTileCell(int32 X, int32 Y)
	{
		return Tiles + (X + Y * Width);
	}

	int32 TileLayer::GetWidth() const
	{
		return Width;
	}

	int32 TileLayer::GetHeight() const
	{
		return Height;
	}

	void TileLayer::BeginEdit()
	{
		bEditing = true;
	}

	void TileLayer::EndEdit()
	{
		bEditing = false;
		OnTileChanged.Broadcast();
	}

	TileLayerPrimitive::TileLayerPrimitive(PrimitiveMobility Mobility, TileLayer* Layer, TileSheet* Sheet, int32 WorldWidth, int32 WorldHeight):
	ScenePrimitive2D(Mobility)
	{
		this->Layer = Layer;
		this->Sheet = Sheet;

		this->WorldWidth = WorldWidth;
		this->WorldHeight = WorldHeight;

		TileItems = new Ry::SharedPtr<BatchItem>[Layer->GetWidth() * Layer->GetHeight()];
		for (int32 I = 0; I < Layer->GetWidth() * Layer->GetHeight(); I++)
			TileItems[I] = MakeItem();
	}

	TileLayerPrimitive::~TileLayerPrimitive()
	{
		delete[] TileItems;
	}

	Texture* TileLayerPrimitive::GetTexture()
	{
		return Sheet->GetTileSheetTexture();
	}

	void TileLayerPrimitive::Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin)
	{
		// todo: use passed in transform to offset/rotate/scale tile layer
		ItemSet->Items.Clear();

		for(int32 TileX = 0; TileX < Layer->GetWidth(); TileX++)
		{
			for (int32 TileY = 0; TileY < Layer->GetHeight(); TileY++)
			{
				
				Ry::SharedPtr<BatchItem> TileItem = TileItems[TileX + TileY * Layer->GetWidth()];

				int32 TileID = Layer->GetTile(TileX, TileY);

				if(TileID != 0)
				{
					TextureRegion TileRegion = Sheet->GetTile(TileID);

					// Batch tile texture
					Ry::BatchTexture(TileItem, WHITE,
						(float) TileX * WorldWidth, (float) TileY * WorldHeight,
						TileRegion.GetUx(), TileRegion.GetVy(),
						TileRegion.GetUw(), TileRegion.GetVh(),
						0.0f, 0.0f,
						(float) WorldWidth, (float)WorldHeight,
						0.0f);

					ItemSet->AddItem(TileItem);
				}

			}
		}
	}

	TileLayerComponent::TileLayerComponent(Entity2D* Owner, PrimitiveMobility Mobility, TileSheet* Sheet, int32 TilesWide, int32 TilesTall, int32 WorldWidth, int32 WorldHeight, bool bCreatePhysics, int32 RenderLayer):
	Primitive2DComponent(Owner, Mobility)
	{
		this->Sheet = Sheet;
		this->Layer = new TileLayer(TilesWide, TilesTall);

		Primitive = MakeShared(new TileLayerPrimitive(Mobility, Layer, Sheet, WorldWidth, WorldHeight));
		Primitive->SetLayer(RenderLayer);

		// Create layer physics component
		if(bCreatePhysics)
		{
			GetOwner()->CreateComponent<TileLayerPhysicsComponent>(Layer, WorldWidth, WorldHeight);
		}
		
	}

	void TileLayerComponent::SetTile(int32 X, int32 Y, int32 TileID)
	{
		Layer->SetTile(X, Y, TileID);

		// todo: regenerate collision
	}

	int32 TileLayerComponent::GetTile(int32 X, int32 Y)
	{
		return Layer->GetTile(X, Y);
	}

	void TileLayerComponent::BeginEdit()
	{
		Layer->BeginEdit();
	}

	void TileLayerComponent::EndEdit()
	{
		Layer->EndEdit();
	}

	TileLayerPhysicsComponent::TileLayerPhysicsComponent(Entity2D* Owner, TileLayer* Layer, int32 WorldWidth, int32 WorldHeight):
	Physics2DComponent(Owner, PhysicsMaterial2D{ Physics2DType::Static, true, 0.0f, 0.0f, {}, false})
	{
		this->Layer = Layer;
		Layer->OnTileChanged.AddMemberFunction(this, &TileLayerPhysicsComponent::OnTileChanged);

		this->WorldWidth = WorldWidth;
		this->WorldHeight = WorldHeight;
	}

	// Returns whether an edge is exposed i.e. not sandwiched between two other tiles
	/*bool TileLayerPhysicsComponent::IsEdgeExposed(Edge* E)
	{
		if (!IsBorderEdge(E))
			return false;
		
		if (!E->TileA || E->TileA->TileID == 0)
			return true;
		if (!E->TileB || E->TileB->TileID == 0)
			return true;
		return false;
	}*/

	bool TileLayerPhysicsComponent::IsBorderEdge(Edge* E)
	{
		return IsCollidable(E->TileA) ^ IsCollidable(E->TileB);
	}

	bool TileLayerPhysicsComponent::IsCollinear(const Node& A, const Node& B)
	{
		// Lines formed from nodes will always be orthogonal and alined with standard 2D basis vectors
		return A.X == B.X || A.Y == B.Y;
	}

	bool TileLayerPhysicsComponent::IsVerticalEdge(Edge* E)
	{
		return E->A.X == E->B.X;
	}	

	bool TileLayerPhysicsComponent::IsCollidable(TileCell* Tile)
	{
		return (Tile != nullptr) && (Tile->TileID != 0);
	}

	bool TileLayerPhysicsComponent::CheckHorEdge(int32 X, int32 Y, Edge* CurEdge)
	{
		if (X < 0 || X >= Layer->GetWidth())
			return false;
		if (Y < 0 || Y >= Layer->GetHeight() + 1)
			return false;

		Edge* Ed = &GeneratedData.HorizontalEdges[X + Y * Layer->GetWidth()];

		if (Ed == CurEdge)
			return false;
		
		if (!IsBorderEdge(Ed))
			return false;

		if (!CurEdge->SharesCollidableTile(Ed))
			return false;

		return true;
	}

	bool TileLayerPhysicsComponent::CheckVertEdge(int32 X, int32 Y, Edge* CurEdge)
	{
		if (X < 0 || X >= Layer->GetWidth() + 1)
			return false;
		if (Y < 0 || Y >= Layer->GetHeight())
			return false;

		Edge* Ed = &GeneratedData.VerticalEdges[X + Y * (Layer->GetWidth() + 1)];

		if (Ed == CurEdge)
			return false;

		if (!IsBorderEdge(Ed))
			return false;

		if (!CurEdge->SharesCollidableTile(Ed))
			return false;

		return true;
	}

	Edge* TileLayerPhysicsComponent::GetHorEdge(int32 X, int32 Y)
	{
		if (X < 0 || X >= Layer->GetWidth())
			return nullptr;
		if (Y < 0 || Y >= Layer->GetHeight() + 1)
			return nullptr;

		return &GeneratedData.HorizontalEdges[X + Y * Layer->GetWidth()];
	}

	Edge* TileLayerPhysicsComponent::GetVertEdge(int32 X, int32 Y)
	{
		if (X < 0 || X >= Layer->GetWidth() + 1)
			return nullptr;
		if (Y < 0 || Y >= Layer->GetHeight())
			return nullptr;

		return &GeneratedData.VerticalEdges[X + Y * (Layer->GetWidth() + 1)];
	}

	Node TileLayerPhysicsComponent::FindNextNode(Node A, Node B, Edge** InOutEdge)
	{
		// go through neighboring edges
		Edge* ResultEdge = nullptr;

		// Check all edges
		if (CheckHorEdge(B.X, B.Y, *InOutEdge))
			ResultEdge = GetHorEdge(B.X, B.Y);
		else if (CheckHorEdge(B.X - 1, B.Y, *InOutEdge))
			ResultEdge = GetHorEdge(B.X - 1, B.Y);
		else if (CheckVertEdge(B.X, B.Y, *InOutEdge))
			ResultEdge = GetVertEdge(B.X, B.Y);
		else if (CheckVertEdge(B.X, B.Y - 1, *InOutEdge))
			ResultEdge = GetVertEdge(B.X, B.Y - 1);

		*InOutEdge = ResultEdge;
		if (B == ResultEdge->A)
			return ResultEdge->B;
		if (B == ResultEdge->B)
			return ResultEdge->A;

		// Should never get here
		assert(false);
		
		return Node{};
	}

	b2ChainShape* TileLayerPhysicsComponent::CreateChain(Edge* StartingEdge)
	{
		// Assume the starting edge is exposed
		b2ChainShape* Chain = new b2ChainShape;
		Ry::ArrayList<b2Vec2> Verts;
		Node StartingNode = IsVerticalEdge(StartingEdge) ? StartingEdge->B : StartingEdge->A; // always go from B to A
		Node PrevNode = StartingNode;
		Node LastPlacedNode = StartingNode;
		Node CurrentNode = IsVerticalEdge(StartingEdge) ? StartingEdge->A : StartingEdge->B;
		
		Edge* CurEdge = StartingEdge;
		CurEdge->bMarked = true;

		Ry::ArrayList<Edge*> Edges;

		auto PlaceNode = [this, &Verts](int32 X, int32 Y)
		{
			// Place current node
			float XMeters = PixelsToMeters((float) (X * WorldWidth));
			float YMeters = PixelsToMeters((float) (Y * WorldHeight));

			Verts.Add(b2Vec2{ XMeters, YMeters });
		};

		PlaceNode(StartingNode.X, StartingNode.Y);

		while (CurrentNode != StartingNode) 
		{
			Edges.Add(CurEdge); // Keep track of edges

			Node NextNode = FindNextNode(PrevNode, CurrentNode, &CurEdge);

			// Color edge
			CurEdge->bMarked = true;

			// Place the node
			PlaceNode(CurrentNode.X, CurrentNode.Y);
			PrevNode = CurrentNode;
			CurrentNode = NextNode;
			LastPlacedNode = CurrentNode; // todo: use this for collinear test
		}

		// Do enclosure test
		bool bEnclosed = true;
		for(Edge* E : Edges)
		{
			bool bVert = IsVerticalEdge(E);
			if(bVert)
			{
				int32 Y = E->A.Y;
				if(E->RightFacing())
				{
					bool bFound = false;
					int32 StartX = E->A.X + 1;
					int32 EndX = Layer->GetWidth();
					while (StartX <= EndX && !bFound)
					{
						Edge* New = GetVertEdge(StartX, Y);
						if (IsBorderEdge(New) && New->LeftFacing())
							bFound = true;
						StartX++;
					}
					if (!bFound)
						bEnclosed = false;
				}
				else
				{
					bool bFound = false;
					int32 StartX = E->B.X - 1;
					int32 EndX = 0;
					while (StartX >= EndX && !bFound)
					{
						Edge* New = GetVertEdge(StartX, Y);
						if (IsBorderEdge(New) && New->RightFacing())
							bFound = true;
						StartX--;
					}
					if (!bFound)
						bEnclosed = false;
				}
			}
			else
			{
				int32 X = E->A.X;
				if (E->UpFacing())
				{
					bool bFound = false;
					int32 StartY = E->A.Y + 1;
					int32 EndY = Layer->GetHeight();
					while (StartY < EndY && !bFound)
					{
						Edge* New = GetHorEdge(X, StartY);
						if (IsBorderEdge(New) && New->DownFacing())
							bFound = true;
						StartY++;
					}
					if (!bFound)
						bEnclosed = false;

				}
				else
				{
					bool bFound = false;
					int32 StartY = E->B.Y - 1;
					int32 EndY = 0;
					while (StartY >= EndY && !bFound)
					{
						Edge* New = GetHorEdge(X, StartY);
						if (IsBorderEdge(New) && New->UpFacing())
							bFound = true;
						StartY--;
					}
					if (!bFound)
						bEnclosed = false;

				}
			}
		}

		// Reverse the winding order
		if(bEnclosed)
		{
			Ry::ArrayList<b2Vec2> Reverse;

			for(int32 Vert = 0; Vert < Verts.GetSize(); Vert++)
			{
				Reverse.Add(Verts[Verts.GetSize() - Vert - 1]);
			}

			Verts = Reverse;			
		}

		Chain->CreateLoop(Verts.GetData(), Verts.GetSize());

		return Chain;
	}

	void TileLayerPhysicsComponent::CreatePhysicsState()
	{
		// Destroy all existing fixtures
		if(Body)
		{
			Ry::ArrayList<b2Fixture*> Fixtures;
			b2Fixture* Fixture = Body->GetFixtureList();
			while (Fixture)
			{
				Fixtures.Add(Fixture);
				Fixture = Fixture->GetNext();
			}

			for (b2Fixture* Fix : Fixtures)
				Body->DestroyFixture(Fix);
		}
		else
		{
			// Create body for first time
			b2World* ParentWorld = GetWorld()->GetPhysicsWorld();

			// Force static and world origin (for now)
			b2BodyDef BodyDef;
			BodyDef.type = b2_staticBody;
			BodyDef.position.Set(0.0f, 0.0f);

			Body = ParentWorld->CreateBody(&BodyDef);
		}

		Ry::ArrayList<b2ChainShape*> ChainShapes;

		// Create edges
		GeneratedData.HorizontalEdges = new Edge[Layer->GetWidth() * (Layer->GetHeight() + 1)];
		GeneratedData.VerticalEdges = new Edge[(Layer->GetWidth() + 1) * (Layer->GetHeight())];
		for(int32 Row = 0; Row <= Layer->GetHeight(); Row++)
		{
			for (int32 Col = 0; Col < Layer->GetWidth(); Col++)
			{
				int32 Index = Col + Row * Layer->GetWidth();
				GeneratedData.HorizontalEdges[Index].A.X = Col;
				GeneratedData.HorizontalEdges[Index].B.X = Col + 1;
				GeneratedData.HorizontalEdges[Index].A.Y = Row;
				GeneratedData.HorizontalEdges[Index].B.Y = Row;

				if (Row <= 0)
					GeneratedData.HorizontalEdges[Index].TileA = nullptr;
				else
					GeneratedData.HorizontalEdges[Index].TileA = Layer->GetTileCell(Col, Row - 1);

				if (Row >= Layer->GetHeight())
					GeneratedData.HorizontalEdges[Index].TileB = nullptr;
				else
					GeneratedData.HorizontalEdges[Index].TileB = Layer->GetTileCell(Col, Row);

			}
		}

		for (int32 Row = 0; Row < Layer->GetHeight(); Row++)
		{
			for (int32 Col = 0; Col <= Layer->GetWidth(); Col++)
			{
				int32 Index = Col + Row * (Layer->GetWidth() + 1);
				GeneratedData.VerticalEdges[Index].A.X = Col;
				GeneratedData.VerticalEdges[Index].B.X = Col;
				GeneratedData.VerticalEdges[Index].A.Y = Row;
				GeneratedData.VerticalEdges[Index].B.Y = Row + 1;

				if (Col <= 0)
					GeneratedData.VerticalEdges[Index].TileA = nullptr;
				else
					GeneratedData.VerticalEdges[Index].TileA = Layer->GetTileCell(Col - 1, Row);

				if (Col >= Layer->GetWidth())
					GeneratedData.VerticalEdges[Index].TileB = nullptr;
				else
					GeneratedData.VerticalEdges[Index].TileB = Layer->GetTileCell(Col, Row);

			}
		}

		// Traverse edges
		for (int32 Row = 0; Row < Layer->GetHeight(); Row++)
		{
			for (int32 Col = 0; Col <= Layer->GetWidth(); Col++)
			{
				int32 Index = Col + Row * Layer->GetWidth();
				Edge* E = &GeneratedData.VerticalEdges[Index];
				if(!E->bMarked && IsBorderEdge(E))
				{
					// generate
					ChainShapes.Add(CreateChain(&GeneratedData.VerticalEdges[Index]));
				}

			}
		}

		for (int32 Row = 0; Row <= Layer->GetHeight(); Row++)
		{
			for (int32 Col = 0; Col < Layer->GetWidth(); Col++)
			{
				int32 Index = Col + Row * Layer->GetWidth();
				Edge* E = &GeneratedData.HorizontalEdges[Index];
				if (!E->bMarked && IsBorderEdge(E))
				{
					// generate
					ChainShapes.Add(CreateChain(&GeneratedData.HorizontalEdges[Index]));
				}

			}
		}

		for(b2ChainShape* ChainShape : ChainShapes)
		{
			b2FixtureDef FixtureDef;
			FixtureDef.userData.pointer = (uintptr_t) this;
			FixtureDef.shape = ChainShape;
			
			Body->CreateFixture(&FixtureDef);
		}

	}

	void TileLayerPhysicsComponent::OnTileChanged()
	{
		// Re-create physics state
		CreatePhysicsState();
	}

	TileLayerEntity::TileLayerEntity(World2D* World, TileSheet* Sheet, int32 Width, int32 Height, int32 WorldWidth, int32 WorldHeight, bool bCreatePhysics, int32 LayerIndex):
	Entity2D(World)
	{
		TileLayer = CreateComponent<TileLayerComponent>(Static, Sheet, Width, Height, WorldWidth, WorldHeight, bCreatePhysics, LayerIndex);
	}

	void TileLayerEntity::SetTile(int32 X, int32 Y, int32 TileID)
	{
		TileLayer->SetTile(X, Y, TileID);
	}

	int32 TileLayerEntity::GetTile(int32 X, int32 Y)
	{
		return TileLayer->GetTile(X, Y);
	}

	void TileLayerEntity::BeginEdit()
	{
		TileLayer->BeginEdit();
	}

	void TileLayerEntity::EndEdit()
	{
		TileLayer->EndEdit();
	}
	
}

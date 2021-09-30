#pragma once

#include "Data/Map.h"
#include "Animation.h"
#include "Scene2D.h"
#include "Component2D.h"
#include "Entity2D.h"

class b2ChainShape;

namespace Ry
{

	struct Node
	{
		int32 X = 0;
		int32 Y = 0;

		bool operator!=(const Node& Other) const
		{
			return !(*this == Other);
		}

		bool operator==(const Node& Other) const
		{
			return X == Other.X && Y == Other.Y;
		}
	};
	
	struct TileCell
	{
		int32 TileID = 0;
		int32 X = 0;
		int32 Y = 0;

		bool IsNeighboar(const TileCell* Other)
		{
			return X == Other->X || Y == Other->Y;
		}
	};

	struct Edge
	{
		Node A;
		Node B;

		TileCell* TileA = nullptr;
		TileCell* TileB = nullptr;

		bool bMarked = false;

		bool RightFacing()
		{
			return (TileA && TileA->TileID != 0) && (!TileB || (TileB && TileB->TileID == 0));
		}

		bool LeftFacing()
		{
			return !RightFacing();
		}

		bool UpFacing()
		{
			return RightFacing();
		}

		bool DownFacing()
		{
			return !UpFacing();
		}


		bool SharesCollidableTile(Edge* Other)
		{
			bool bShare = TileA && Other->TileA && TileA->IsNeighboar(Other->TileA) && TileA->TileID != 0 && Other->TileA != 0;
			bShare |= TileB && Other->TileB && TileB->IsNeighboar(Other->TileB) && TileB->TileID != 0 && Other->TileB != 0;
			bShare |= TileA && Other->TileB && TileA->IsNeighboar(Other->TileB) && TileA->TileID != 0 && Other->TileB != 0;
			bShare |= TileB && Other->TileA && TileB->IsNeighboar(Other->TileA) && TileB->TileID != 0 && Other->TileA != 0;

			return bShare;
		}
	};

	// Data for tile layer
	class SCENE2D_MODULE TileLayer
	{
	public:

		MulticastDelegate<> OnTileChanged;
		
		TileLayer(int32 TilesWide, int32 TilesTall);
		~TileLayer();

		void SetTile(int32 X, int32 Y, int32 ID);
		int32 GetTile(int32 X, int32 Y) const;

		TileCell* GetTileCell(int32 X, int32 Y);

		int32 GetWidth() const;
		int32 GetHeight() const;

		void BeginEdit();
		void EndEdit();

	private:

		bool bEditing;

		TileCell* Tiles;
		int32 Width;
		int32 Height;
	};

	class SCENE2D_MODULE TileSheet
	{
	public:

		TileSheet(Texture* Tiles, int32 TileWidth, int32 TileHeight);

		void RegisterTile(int32 TileID, int32 TilesheetX, int32 TilesheetY);

		TextureRegion GetTile(int32 TileID);

		Texture* GetTileSheetTexture() const;

		int32 GetTileWidth() const;
		int32 GetTileHeight() const;

	private:
		Ry::OAHashMap<int32, TextureRegion> Tiles;
		Texture* Tilesheet;

		int32 TileWidth;
		int32 TileHeight;
	};

	class SCENE2D_MODULE TileLayerPrimitive : public ScenePrimitive2D
	{
	public:

		TileLayerPrimitive(PrimitiveMobility Mobility, TileLayer* Layer, TileSheet* Sheet, int32 WorldWidth, int32 WorldHeight);
		~TileLayerPrimitive();

		Texture* GetTexture() override;

		void Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin) override;

	private:

		TileLayer* Layer;
		TileSheet* Sheet;

		int32 WorldWidth;
		int32 WorldHeight;

		Ry::SharedPtr<BatchItem>* TileItems;

	};

	class SCENE2D_MODULE TileLayerComponent : public Primitive2DComponent
	{
	public:

		TileLayerComponent(Entity2D* Owner, PrimitiveMobility Mobility, TileSheet* Sheet, int32 TilesWide, int32 TilesTall, int32 WorldWidth, int32 WorldHeight, bool bCreatePhysics);

		void SetTile(int32 X, int32 Y, int32 TileID);
		int32 GetTile(int32 X, int32 Y);

		void BeginEdit();
		void EndEdit();

	private:
		TileSheet* Sheet;
		TileLayer* Layer;
	};

	class SCENE2D_MODULE TileLayerPhysicsComponent : public Physics2DComponent
	{
	public:

		TileLayerPhysicsComponent(Entity2D* Owner, TileLayer* Layer, int32 WorldWidth, int32 WorldHeight);

		void CreatePhysicsState() override;

		b2ChainShape* CreateChain(Edge* StartingEdge);

		// Returns whether an edge is exposed i.e. not sandwiched between two other tiles
		bool IsBorderEdge(Edge* E);
		bool IsCollinear(const Node& A, const Node& B);
		bool IsVerticalEdge(Edge* E);
		bool IsCollidable(TileCell* Tile);
		bool CheckHorEdge(int32 X, int32 Y, Edge* CurEdge);
		bool CheckVertEdge(int32 X, int32 Y, Edge* CurEdge);
		Edge* GetHorEdge(int32 X, int32 Y);
		Edge* GetVertEdge(int32 X, int32 Y);
		Node FindNextNode(Node A, Node B, Edge** InOutEdge);

	private:

		struct TilePhysicsGenerationData
		{
			Edge* HorizontalEdges;
			Edge* VerticalEdges;
		} GeneratedData;		

		void OnTileChanged();
		
		TileLayer* Layer;

		int32 WorldWidth;
		int32 WorldHeight;
	};

	class SCENE2D_MODULE TileLayerEntity : public Entity2D
	{
	public:

		TileLayerEntity(World2D* World, 
			TileSheet* Sheet, 
			int32 Width, int32 Height, 
			int32 WorldWidth, int32 WorldHeight, 
			bool bCreatePhysics);

		void SetTile(int32 X, int32 Y, int32 TileID);
		int32 GetTile(int32 X, int32 Y);

		void BeginEdit();
		void EndEdit();

	private:
		SharedPtr<TileLayerComponent> TileLayer;
	};

	
	
}

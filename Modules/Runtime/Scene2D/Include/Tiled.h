#pragma once

#include "Asset.h"
#include "Factory/AssetFactory.h"
#include "TextureAsset.h"
#include "Data/Map.h"
#include "Tiled.gen.h"

namespace Ry
{

	struct SCENE2D_MODULE TmxTileSheet
	{
		Ry::String Name;
		int32 FirstGid = -1;
		int32 TileWidth = 0;
		int32 TileHeight = 0;
		int32 TileCount = 0;
		int32 Columns = 0;
		TextureAsset* Image = nullptr;

		bool operator>(const TmxTileSheet& Other)
		{
			return FirstGid > Other.FirstGid;
		}
	};

	struct TmxPoint
	{
		OAHashMap<Ry::String, Ry::String> Properties;
		Ry::String Name;
		float X;
		float Y;
	};

	struct TmxRect
	{
		OAHashMap<Ry::String, Ry::String> Properties;
		Ry::String Name;
		float X;
		float Y;
		float W;
		float H;
	};

	struct TmxText
	{
		OAHashMap<Ry::String, Ry::String> Properties;
		Ry::String Value;
		float X;
		float Y;
		float Width;
		float Height;
		int32 FontSize;
	};

	struct SCENE2D_MODULE TmxObjectLayer
	{
		Ry::String Name;

		OAHashMap<Ry::String, Ry::String> Properties;

		Ry::ArrayList<TmxText> TextObjects;
		Ry::ArrayList<TmxRect> RectObjects;
		Ry::ArrayList<TmxPoint> PointObjects;

	};

	struct SCENE2D_MODULE TmxTileLayer
	{
		Ry::String Name;
		int32 Width;
		int32 Height;
		int32* TileGuids = nullptr;

		OAHashMap<Ry::String, Ry::String> Properties;
	};

	struct SCENE2D_MODULE TmxMap
	{
		int32 Width;
		int32 Height;

		int32 TileWidth;
		int32 TileHeight;

		Ry::ArrayList<TmxObjectLayer> TmxObjectLayers;
		Ry::ArrayList<TmxTileLayer> TmxLayers;
		Ry::ArrayList<TmxTileSheet> TmxTileSheets;
	};

	class SCENE2D_MODULE TmxMapFactory : public AssetFactory
	{
	public:
		void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;

	private:

		TmxMap LoadTmxMap(const Ry::AssetRef& Reference);
	};

	class SCENE2D_MODULE TmxMapAsset : public Asset
	{
	public:

		TmxMapAsset();
		TmxMapAsset(const TmxMap& Asset);
		virtual ~TmxMapAsset();

		TmxMap& GetMap();

		void UnloadAsset() override {};

	private:
		
		TmxMap Map;

	};

}

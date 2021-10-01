#include "Tiled.h"

#include "rapidxml.hpp"
#include "File/File.h"
#include "Manager/AssetManager.h"

using namespace rapidxml;

namespace Ry
{

	void TmxMapFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{
		TmxMap Res = LoadTmxMap(Reference);
		TmxMapAsset* TmxAsset = new TmxMapAsset(Res);
		AssetDst.push_back(TmxAsset);
	}

	TmxLayer LoadTmxLayer(xml_node<>* Node)
	{
		TmxLayer Result;

		Result.Width = Ry::ParseInt(Node->first_attribute("width")->value());
		Result.Height = Ry::ParseInt(Node->first_attribute("height")->value());
		Result.Name = Node->first_attribute("name")->value();

		Result.TileGuids = new int32[Result.Width * Result.Height];

		Ry::String Csv = Node->first_node("data")->value();
		Ry::StringView* Values = nullptr;
		int32 Tokens = Csv.split(",", &Values);
		{
			for (int32 Tile = 0; Tile < Tokens; Tile++)
			{
				Result.TileGuids[Tile] = Ry::ParseInt(Values[Tile].GetData());
			}
		}

		delete[] Values;

		return Result;
	}

	TmxTileSheet LoadTmxTileSheet(const Ry::AssetRef& Reference, xml_node<>* Node)
	{
		Ry::String Parent = Ry::File::GetParentPath(Reference.GetAbsolute());

		TmxTileSheet Result;
		
		Result.FirstGid = Ry::ParseInt(Node->first_attribute("firstgid")->value());

		// Check if the tms file is somewhere else
		xml_node<>* Root = nullptr;
		xml_attribute<>* Src = Node->first_attribute("source");
		xml_document<> AltDoc;
		if(Src)
		{
			Ry::String TmsPath = Ry::File::Join(Parent, Src->value());
			Ry::String Contents = Ry::File::LoadFileAsString2(TmsPath);
			AltDoc.parse<0>(*Contents);
			
			Root = AltDoc.first_node("tileset");
		}
		else
		{
			Root = Node;
		}

		if(Root)
		{
			Result.Name = Root->first_attribute("name")->value();
			Result.TileWidth = Ry::ParseInt(Root->first_attribute("tilewidth")->value());
			Result.TileHeight = Ry::ParseInt(Root->first_attribute("tileheight")->value());
			Result.TileCount = Ry::ParseInt(Root->first_attribute("tilecount")->value());
			Result.Columns = Ry::ParseInt(Root->first_attribute("columns")->value());

			// load image
			xml_node<>* Image = Root->first_node("image");
			if(Image)
			{
				Ry::String Path = Image->first_attribute("source")->value();
				Ry::String Combined = Ry::File::Join(Parent, Path);
				Ry::String NewVirtual = Ry::File::AbsoluteToVirtual(Combined);
				Result.Image = Ry::AssetMan->GetOrLoadAsset<TextureAsset>(NewVirtual, "image");
			}
			else
			{
				std::cerr << "Tilesheet " << *Result.Name << " did not have image" << std::endl;
			}

			// todo: load wang sets?
		}

		return Result;
	}

	TmxMap TmxMapFactory::LoadTmxMap(const Ry::AssetRef& Reference)
	{
		TmxMap Result;

		Ry::String Contents = Ry::File::LoadFileAsString2(Reference.GetAbsolute());
		xml_document<> TmxDoc;
		TmxDoc.parse<0>(*Contents);

		xml_node<>* MapNode = TmxDoc.first_node();
		Result.Width = Ry::ParseInt(MapNode->first_attribute("width")->value());
		Result.Height = Ry::ParseInt(MapNode->first_attribute("height")->value());
		Result.TileWidth = Ry::ParseInt(MapNode->first_attribute("tilewidth")->value());
		Result.TileHeight = Ry::ParseInt(MapNode->first_attribute("tileheight")->value());

		// Load layers
		xml_node<>* CurNode = MapNode->first_node();
		while(CurNode)
		{
			Ry::String NodeName = CurNode->name();
			if(NodeName == "layer")
			{
				TmxLayer NewLayer = LoadTmxLayer(CurNode);
				Result.TmxLayers.Add(NewLayer);
			}
			else if(NodeName == "tileset")
			{
				TmxTileSheet NewSheet = LoadTmxTileSheet(Reference, CurNode);

				// Guarantee tmx maps are sorted
				int32 CurIndex = 0;
				while (CurIndex < Result.TmxTileSheets.GetSize() && NewSheet > Result.TmxTileSheets[CurIndex])
					CurIndex++;

				Result.TmxTileSheets.Insert(NewSheet, CurIndex);
			}
			
			CurNode = CurNode->next_sibling();
		}

		return Result;
	}

	TmxMapAsset::TmxMapAsset()
	{
				
	}

	TmxMapAsset::TmxMapAsset(const TmxMap& Asset)
	{
		this->Map = Asset;
	}

	TmxMapAsset::~TmxMapAsset()
	{
		
	}

	TmxMap& TmxMapAsset::GetMap()
	{
		return Map;
	}
	
}

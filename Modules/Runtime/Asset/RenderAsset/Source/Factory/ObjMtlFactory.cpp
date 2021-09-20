#include "Factory/ObjMtlFactory.h"
#include "MaterialAsset.h"
#include "Factory/ObjMeshFactory.h"
#include "TextureAsset.h"
#include "Manager/IAssetManager.h"
#include "File/File.h"
#include "Core/Globals.h"
#include "Interface/Texture.h"

namespace Ry
{
	
	Ry::String ObjMtlFactory::BuildName(int32 BeginToken, int32 TokenCount, Ry::String** Tokens)
	{
		Ry::String Name = "";
		for (int32 RemainingTokens = BeginToken; RemainingTokens < TokenCount; RemainingTokens++)
		{
			Name += (*Tokens)[RemainingTokens];

			if (TokenCount - RemainingTokens > 1)
				Name += " ";
		}

		return Name;
	}

	int32 ExtractTokens(Ry::StringView* Input, int32 Size, Ry::String** Result)
	{
		*Result = new Ry::String[Size];
		int32 ValidIndex = 0;

		for (int32 Index = 0; Index < Size; Index++)
		{
			if (Input[Index].getSize() > 0)
			{
				(*Result)[ValidIndex] = Input[Index].Trim();
				//std::cout << "Extracted " << Result[ValidIndex]->getData() << std::endl;

				ValidIndex++;
			}
		}

		return ValidIndex;
	}
	
	void ObjMtlFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{		
		Ry::StringView* MatLibLines = nullptr;
		Ry::String MatLibContents = Ry::File::LoadFileAsString2(Reference.GetAbsolute());
		int32 MatLibLineCount = MatLibContents.split("\n", &MatLibLines);

		int32 MaterialIndex = 0;
		Material* CurrentMaterial = nullptr;

		// Load the OBJ materials
		for (int32 Line = 0; Line < MatLibLineCount; Line++)
		{
			StringView* RawTokens = nullptr;
			String LineContents = MatLibLines[Line];
			int32 RawTokenCount = LineContents.split(" ", &RawTokens);

			String* ValidTokens = nullptr;
			int32 ValidTokenCount = ExtractTokens(RawTokens, RawTokenCount, &ValidTokens);

			if (ValidTokenCount > 0)
			{
				if (ValidTokens[0] == "newmtl")
				{
					if (CurrentMaterial)
					{
						AssetDst.push_back(new MaterialAsset(CurrentMaterial));
					}

					CurrentMaterial = new Material;
					CurrentMaterial->Name = BuildName(1, ValidTokenCount, &ValidTokens);
				}

				if (CurrentMaterial)
				{
					if (ValidTokens[0] == "Ka")
					{
						// TODO: what do we translate this to?
						// CurrentMaterial->Albedo.r = ValidTokens[1].to_float();
						// CurrentMaterial->Ambient.g = ValidTokens[2].to_float();
						// CurrentMaterial->Ambient.b = ValidTokens[3].to_float();
					}
					if (ValidTokens[0] == "Kd")
					{
						CurrentMaterial->Albedo.r = ValidTokens[1].to_float();
						CurrentMaterial->Albedo.g = ValidTokens[2].to_float();
						CurrentMaterial->Albedo.b = ValidTokens[3].to_float();
					}
					if (ValidTokens[0] == "Ks")
					{
						// TODO: what do we translate this to?
						// CurrentMaterial->Specular.r = ValidTokens[1].to_float();
						// CurrentMaterial->Specular.g = ValidTokens[2].to_float();
						// CurrentMaterial->Specular.b = ValidTokens[3].to_float();
					}

					// Ambient texture map
					if (ValidTokens[0] == "map_Ka")
					{

					}

					// Diffuse texture map
					if (ValidTokens[0] == "map_Kd")
					{
						Ry::String Path = ValidTokens[1];
						Ry::String VirtualRoot = Ry::File::GetParentPath(Reference.GetVirtual());
						Ry::String NewVirtual = Ry::File::Join(VirtualRoot, Path);

						NewVirtual.Replace('\\', '/');

						// Todo: Texture support needs to be re-added
						Texture* Result = Ry::AssetMan->LoadAsset<TextureAsset>(NewVirtual, "image")->CreateRuntimeTexture();
						CurrentMaterial->AlbedoMap = Result;
					}


				}
			}

		}

		// Flush out last mat

		if (CurrentMaterial)
		{
			AssetDst.push_back(new MaterialAsset(CurrentMaterial));
		}
	}
	
}

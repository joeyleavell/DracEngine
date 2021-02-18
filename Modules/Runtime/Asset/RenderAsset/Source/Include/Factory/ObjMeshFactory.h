#pragma once

#include "Factory/AssetFactory.h"
#include <vector>
#include "RenderAssetGen.h"

namespace Ry
{
	struct Material;

	struct OBJIndex
	{
		int32 VertexIndex = 0;
		int32 TexCoordIndex = 0;
		int32 NormalIndex = 0;
	};

	struct OBJFace
	{
		OBJIndex First;
		OBJIndex Second;
		OBJIndex Third;
		OBJIndex Fourth;

		bool IsTriangle = true;
	};

	struct OBJMaterialGroup
	{
		Material* Mat;
		std::vector<OBJFace> Faces;

		OBJMaterialGroup():
		Mat(nullptr)
		{
			
		}
	};
	
	class RENDERASSET_MODULE ObjMeshFactory : public AssetFactory
	{
		void LoadMaterialLib(const Ry::String Path, std::vector<Material*>& Materials);

		int32 ExtractTokens(Ry::StringView* Input, int32 Size, Ry::StringView** Result);
		
		OBJIndex ParseIndex(const Ry::StringView& Index);
		OBJFace ParseFace(const Ry::StringView& First, const Ry::StringView& Second, const Ry::StringView& Third);
		
		void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;

		Ry::String BuildName(int32 BeginToken, int32 TokenCount, Ry::StringView** Tokens);
		
	};
}

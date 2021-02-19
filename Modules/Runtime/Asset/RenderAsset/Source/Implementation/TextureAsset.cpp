#include "TextureAsset.h"
#include "Core/Globals.h"
#include "Bitmap.h"
#include "Interface2/RenderAPI.h"
#include "Interface2/Texture2.h"

namespace Ry
{

	TextureAsset::TextureAsset(Bitmap* Resource)
	{
		this->Resource = Resource;
	}

	Texture2* TextureAsset::CreateRuntimeTexture()
	{
		Texture2* NewTexture = Ry::NewRenderAPI->CreateTexture();
		NewTexture->Data(Resource);

		RuntimeResources.Add(NewTexture);

		return NewTexture;
	}
	
	void TextureAsset::UnloadAsset()
	{
		for(Texture2* RuntimeResource : RuntimeResources)
		{
			RuntimeResource->DeleteTexture();
			delete RuntimeResource;
		}
		
		delete Resource;
	}
	
}

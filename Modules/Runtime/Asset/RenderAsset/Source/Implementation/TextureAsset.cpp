#include "TextureAsset.h"
#include "Core/Globals.h"
#include "Bitmap.h"
#include "Interface/RenderAPI.h"
#include "Interface/Texture.h"

namespace Ry
{

	TextureAsset::TextureAsset(Bitmap* Resource)
	{
		this->Resource = Resource;
	}

	Texture* TextureAsset::CreateRuntimeTexture()
	{
		Texture* NewTexture = Ry::RendAPI->CreateTexture();
		NewTexture->Data(Resource);

		RuntimeResources.Add(NewTexture);

		return NewTexture;
	}

	Ry::Bitmap* TextureAsset::GetResource()
	{
		return Resource;
	}

	void TextureAsset::UnloadAsset()
	{
		for(Texture* RuntimeResource : RuntimeResources)
		{
			RuntimeResource->DeleteTexture();
			delete RuntimeResource;
		}
		
		delete Resource;
	}
	
}

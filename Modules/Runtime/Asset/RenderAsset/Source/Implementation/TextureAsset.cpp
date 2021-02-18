#include "TextureAsset.h"
#include "Core/Globals.h"
#include "Bitmap.h"
#include "Interface/Rendering.h"

namespace Ry
{

	TextureAsset::TextureAsset(Bitmap* Resource)
	{
		this->Resource = Resource;
	}

	Texture* TextureAsset::CreateRuntimeTexture()
	{
		Texture* NewTexture = Ry::RenderAPI->make_texture();
		NewTexture->Data(Resource);

		return NewTexture;
	}
	
	void TextureAsset::UnloadAsset()
	{
		delete Resource;
	}
	
}
#pragma once

#include "Core/Core.h"
#include "Factory/AssetFactory.h"
#include "AudioGen.h"

namespace FMOD
{
	class Sound;
}

namespace Ry
{

	class AUDIO_MODULE AudioFactory : public AssetFactory
	{
	public:

		void LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst) override;

	private:


	};

	
}

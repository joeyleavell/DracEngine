#include "Factory/AudioFactory.h"
// #include "FMod/fmod.hpp"
// #include "FMod/fmod_errors.h"
// #include "Audio/Audio.h"
#include "AudioAsset.h"

namespace Ry
{

	void AudioFactory::LoadAssets(const Ry::AssetRef& Reference, std::vector<Asset*>& AssetDst)
	{
		/*FMOD::Sound* InternalAsset;
		FmodSystem->createSound(*Reference.GetAbsolute(), FMOD_DEFAULT, nullptr, &InternalAsset);

		AudioAsset* Result = new AudioAsset;
		Result->SetSound(InternalAsset);

		AssetDst.push_back(Result);*/
	}

}

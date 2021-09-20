#pragma once

#include "Asset.h"
#include "AudioGen.h"

namespace FMOD
{
	class Sound;
	class Channel;
}

namespace Ry
{

	class AUDIO_MODULE AudioAsset : public Asset
	{
	public:

		AudioAsset()
		{
			this->InternalAsset = nullptr;
			this->PlayingChannel = nullptr;
		}
		
		void UnloadAsset() override
		{
			
		}

		void SetSound(FMOD::Sound* Sound)
		{
			this->InternalAsset = Sound;
		}

		void Play();

	private:

		FMOD::Sound* InternalAsset;
		FMOD::Channel* PlayingChannel;
	};
	
}

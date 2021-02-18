#pragma once
#include "AudioGen.h"

namespace FMOD
{
	class System;
}

namespace Ry
{

	AUDIO_MODULE void InitAudio();
	AUDIO_MODULE void UpdateAudio();
	AUDIO_MODULE void PlayAudio();

	extern FMOD::System* FmodSystem;
}

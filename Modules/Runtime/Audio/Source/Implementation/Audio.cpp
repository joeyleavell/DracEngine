
#include "Audio.h"

//#include "FMod/fmod.hpp"
//#include "FMod/fmod_errors.h"
#include <iostream>

namespace Ry
{

	FMOD::System* FmodSystem;
	
	void InitAudio()
	{
		/*if(FMOD::System_Create(&FmodSystem) != FMOD_OK)
		{
			
			std::cerr << "Problem setting up FMOD!" << std::endl;
		}
		else
		{
			FmodSystem->init(36, FMOD_INIT_NORMAL, NULL);
		}*/
	}

	void UpdateAudio()
	{
		//FmodSystem->update();
	}
	
}
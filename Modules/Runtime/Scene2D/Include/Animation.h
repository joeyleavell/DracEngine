#pragma once

#include "Core/Core.h"
#include "Interface/Texture.h"
#include "Animation.gen.h"

namespace Ry
{

	struct TextureRegion
	{
		int32 X;
		int32 Y;
		int32 W;
		int32 H;
		Texture* Parent;

		float GetUx()
		{
			return X / (float)Parent->GetWidth();
		}

		float GetVy()
		{
			return Y / (float)Parent->GetHeight();
		}

		float GetUw()
		{
			return W / (float)Parent->GetWidth();
		}

		float GetVh()
		{
			return H / (float)Parent->GetHeight();
		}


	};
	
}

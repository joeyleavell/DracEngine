#pragma once

#include "Core/Core.h"
#include "Interface/Texture.h"
#include "Animation.gen.h"

namespace Ry
{

	struct SCENE2D_MODULE TextureRegion
	{
		int32 X;
		int32 Y;
		int32 W;
		int32 H;
		Texture* Parent;

		TextureRegion()
		{
			this->X = 0;
			this->Y = 0;
			this->W = 0;
			this->H = 0;
			this->Parent = nullptr;
		}

		TextureRegion(Texture* Parent)
		{
			this->X = 0;
			this->Y = 0;
			this->W = Parent->GetWidth();
			this->H = Parent->GetHeight();
			this->Parent = Parent;
		}


		TextureRegion(int32 X, int32 Y, int32 W, int32 H, Texture* Parent)
		{
			this->X = X;
			this->Y = Y;
			this->W = W;
			this->H = H;
			this->Parent = Parent;
		}

		float GetUx()
		{
			return (X / (float)Parent->GetWidth()) + 0.001f;
		}

		float GetVy()
		{
			return (Y / (float)Parent->GetHeight()) + 0.001f;
		}

		float GetUw()
		{
			return (W / (float)Parent->GetWidth()) - 2 * 0.001f;
		}

		float GetVh()
		{
			return (H / (float)Parent->GetHeight()) - 2 * 0.001f;
		}
	};

	class SCENE2D_MODULE Animation
	{
	public:

		// @param Speed Length of each frame in seconds
		Animation(Texture* Parent, float Speed);

		void AddFrame(int32 X, int32 Y, int32 W, int32 H);
		TextureRegion& GetFrame(int32 Index);

		float GetSpeed() const;

		int32 GetNumFrames() const;

		Texture* GetParent() const;

	private:
		Ry::ArrayList<TextureRegion> Frames;
		Texture* Parent;
		float Speed;
	};

	SCENE2D_MODULE SharedPtr<Animation> CreateAnimation(Texture* Parent, float Speed, int32 StartX, int32 StartY, int32 FrameWidth, int32 FrameHeight, int32 FramesWide, int32 FramesHigh);

}

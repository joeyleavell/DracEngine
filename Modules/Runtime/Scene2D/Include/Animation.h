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

		float GetUx()
		{
			return (X / (float)Parent->GetWidth()) + 0.01f;
		}

		float GetVy()
		{
			return (Y / (float)Parent->GetHeight()) + 0.01f;
		}

		float GetUw()
		{
			return (W / (float)Parent->GetWidth()) - 2 * 0.01f;
		}

		float GetVh()
		{
			return (H / (float)Parent->GetHeight()) - 2 * 0.01f;
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

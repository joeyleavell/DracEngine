#include "Animation.h"

namespace Ry
{
	Animation::Animation(Texture* Parent, float Speed)
	{
		this->Parent = Parent;
		this->Speed = Speed;
	}

	void Animation::AddFrame(int32 X, int32 Y, int32 W, int32 H)
	{
		Frames.Add(TextureRegion{ X, Y, W, H, Parent });
	}

	TextureRegion& Animation::GetFrame(int32 Index)
	{
		return Frames[Index];
	}

	float Animation::GetSpeed() const
	{
		return Speed;
	}

	int32 Animation::GetNumFrames() const
	{
		return Frames.GetSize();
	}

	Texture* Animation::GetParent() const
	{
		return Parent;
	}

	SharedPtr<Animation> CreateAnimation(Texture* Parent, float Speed, int32 StartX, int32 StartY, int32 FrameWidth,
		int32 FrameHeight, int32 FramesWide, int32 FramesHigh)
	{
		SharedPtr<Animation> Anim = MakeShared(new Animation{ Parent, Speed });

		for(int32 YFrame = 0; YFrame < FramesHigh; YFrame++)
		{
			for (int32 XFrame = 0; XFrame < FramesWide; XFrame++)
			{
				int32 PixelX = StartX + XFrame * FrameWidth;
				int32 PixelY = StartY + YFrame * FrameHeight;

				Anim->AddFrame(PixelX, PixelY, FrameWidth, FrameHeight);
			}
		}

		return Anim;
	}
}

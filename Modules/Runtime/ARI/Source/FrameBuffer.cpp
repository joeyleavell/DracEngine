#include "FrameBuffer.h"

namespace Ry
{
	
	FrameBuffer::FrameBuffer(uint32 Width, uint32 Height, const RenderPass* RenderPass, const FrameBufferDescription* FbDesc):
	IntendedWidth(Width), IntendedHeight(Height)
	{
		if(FbDesc)
		{
			Description = *FbDesc;
		}
	};
	
}
#include "RenderPass.h"

namespace Ry
{
	const FrameBufferDescription& RenderPass::GetFboDescription() const
	{
		return Description;
	}
}

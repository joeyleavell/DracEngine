#pragma once

#include "Core/Core.h"
#include "RenderingGen.h"

namespace Ry
{

	class SwapChain;

	enum class AttachmentFormat
	{
		/** The same internal format that the target swap chain uses. */
		Color,
		Depth,
		Stencil
	};

	class AttachmentDescription
	{
	public:

		int32 Samples = 1;
		AttachmentFormat Format;
		SwapChain* ReferencingSwapChain = nullptr;
	};

	class FrameBufferDescription
	{
	public:

		Ry::ArrayList<AttachmentDescription> Attachments;

		int32 AddSwapChainColorAttachment(SwapChain* SC)
		{
			AttachmentDescription NewDesc;
			NewDesc.Format = AttachmentFormat::Color;
			NewDesc.ReferencingSwapChain = SC;

			Attachments.Add(NewDesc);

			return Attachments.GetSize() - 1;
		}

		int32 AddSwapChainDepthAttachment(SwapChain* SC)
		{
			AttachmentDescription NewDesc;
			NewDesc.Format = AttachmentFormat::Depth;
			NewDesc.ReferencingSwapChain = SC;

			Attachments.Add(NewDesc);

			return Attachments.GetSize() - 1;
		}

		int32 AddColorAttachment()
		{
			AttachmentDescription NewDesc;
			NewDesc.Format = AttachmentFormat::Color;

			Attachments.Add(NewDesc);

			return Attachments.GetSize() - 1;
		}

		int32 AddDepthAttachment()
		{
			AttachmentDescription NewDesc;
			NewDesc.Format = AttachmentFormat::Depth;

			Attachments.Add(NewDesc);

			return Attachments.GetSize() - 1;
		}
	};

	class RENDERING_MODULE Subpass
	{
	public:
		Ry::ArrayList<int32> UsedAttachments;
		int32 SubpassIndex;

	private:
		
	};

	// todo: rename render pass "render graph" and subpass "render node" -- i like that naming scheme better for a generic system
	class RENDERING_MODULE RenderPass
	{
	public:

		RenderPass(): NextSubpassIndex(0){};
		virtual ~RenderPass() = default;

		void SetFramebufferDescription(const FrameBufferDescription& Desc)
		{
			this->Description = Desc;
		}

		int32 CreateSubpass()
		{
			Subpass* NewSub = new Subpass;
			NewSub->SubpassIndex = NextSubpassIndex;
			
			NextSubpassIndex++;

			SubPasses.Add(NewSub);

			return NewSub->SubpassIndex;
		}

		void AddSubpassAttachment(int32 Subpass, int32 Attachment)
		{
			SubPasses[Subpass]->UsedAttachments.Add(Attachment);
		}

		virtual bool CreateRenderPass() = 0;

		virtual void DeleteRenderPass()
		{
			for(Ry::Subpass* Sub : SubPasses)
			{
				delete Sub;
			}
		}

	protected:

		FrameBufferDescription Description;
		Ry::ArrayList<Subpass*> SubPasses;
		int32 NextSubpassIndex;
		
	};
	
}
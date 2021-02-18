#pragma once

#include "Core/Core.h"
#include "Interface/Texture.h"
#include "RenderingCoreGen.h"

namespace Ry
{
	class Texture;
	
	constexpr uint32 MAX_COLOR_ATTACHMENTS = 8;

	enum class AttachmentType
	{
		COLOR, DEPTH, STENCIL
	};

	class RENDERINGCORE_MODULE FrameBuffer
	{

	public:
		FrameBuffer(int32 Width, int32 Height, int32 Samples);
		virtual ~FrameBuffer() = default;

		/// BEGIN INTERFACE
		virtual void Bind() = 0;

		virtual void ClearAll(float Red, float Green, float Blue, float Alpha) = 0;
		virtual void SetColorAttachment(Texture* Texture, int32 AttachPoint) = 0;
		virtual void SetDepthAttachment(Texture* Texture) = 0;
		virtual void SetStencilAttachment(Texture* Texture) = 0;
		virtual Texture* CreateColorTexture() = 0;
		virtual Texture* CreateDepthTexture() = 0;
		virtual Texture* CreateStencilTexture() = 0;

		virtual void BlitFramebuffer(const FrameBuffer* Dst, bool Color, bool Depth, bool Stencil, int32 ReadColorIndex, int32 WriteColorIndex) = 0;
		/// END INTERFACE
		///

		virtual void ResizeBuffer(int32 IntendedWidth, int32 IntendedHeight)
		{
			this->IntendedWidth = IntendedWidth;
			this->IntendedHeight = IntendedHeight;

			// Recreate attachments using new intended size
			
			if (HasStencilAttachment())
			{
				delete GetStencilAttachment();
				StencilAttachment = nullptr;
				
				CreateStencilAttachment();
			}

			if (HasDepthAttachment())
			{
				delete GetDepthAttachment();
				DepthAttachment = nullptr;
				
				CreateDepthAttachment();
			}
			ColorAttachmentCount = 0;

			for (uint32 ColorAttachment = 0; ColorAttachment < MAX_COLOR_ATTACHMENTS; ColorAttachment++)
			{
				if (HasColorAttachment(ColorAttachment))
				{
					delete GetColorAttachment(ColorAttachment);
					ColorAttachments[ColorAttachment] = nullptr;
					
					CreateColorAttachment(ColorAttachment);
				}
			}
		}

		virtual void CreateColorAttachment(int32 ColorIndex)
		{
			SetColorAttachment(CreateColorTexture(), ColorIndex);
		}
		
		virtual void CreateDepthAttachment()
		{
			SetDepthAttachment(CreateDepthTexture());
		}
		
		virtual void CreateStencilAttachment()
		{
			SetStencilAttachment(CreateStencilTexture());
		}

		bool HasColorAttachment(int32 Index)
		{
			return ColorAttachments[Index] != nullptr;
		}

		bool HasDepthAttachment()
		{
			return DepthAttachment != nullptr;
		}

		bool HasStencilAttachment()
		{
			return StencilAttachment != nullptr;
		}

		virtual Texture* GetColorAttachment(int32 Index) const
		{
			return ColorAttachments[Index];
		}

		virtual Texture* GetDepthAttachment() const
		{
			return DepthAttachment;
		}

		virtual Texture* GetStencilAttachment() const
		{
			return StencilAttachment;
		}

		int32 GetIntendedWidth() const
		{
			return IntendedWidth;
		}

		int32 GetIntendedHeight() const
		{
			return IntendedHeight;
		}

		int32 GetSamples() const
		{
			return Samples;
		}

	protected:

		int32 IntendedWidth;
		int32 IntendedHeight;
		int32 Samples;
		
		Texture* ColorAttachments[MAX_COLOR_ATTACHMENTS];
		Texture* DepthAttachment;
		Texture* StencilAttachment;
		int32 ColorAttachmentCount;
	};
	
}

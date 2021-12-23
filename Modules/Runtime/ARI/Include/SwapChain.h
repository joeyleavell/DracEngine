#pragma once

#include "Core/Core.h"
#include "SwapChain.gen.h"

struct GLFWwindow;

namespace Ry
{

	class RenderPass;
	class FrameBuffer;
	class CommandBuffer;

	class ARI_MODULE SwapChain
	{
	public:
		Ry::MulticastDelegate<> OnSwapChainDirty;
		
		SwapChain(){};
		virtual ~SwapChain() = default;

		virtual void BeginFrame(::GLFWwindow* Window, bool bWindowDirty) = 0;
		virtual void EndFrame(::GLFWwindow* Window) = 0;

		virtual bool CreateSwapChain(::GLFWwindow* ParentWindow) = 0;
		virtual void DeleteSwapChain() = 0;

		virtual int32 GetSwapChainWidth() const = 0;
		virtual int32 GetSwapChainHeight() const = 0;

		virtual RenderPass* GetDefaultRenderPass() = 0;
		virtual FrameBuffer* GetDefaultFrameBuffer() = 0;
		
	};
	
}
#pragma once

#include "SwapChain.h"
#include "OpenGL2Gen.h"

namespace Ry
{

	class OPENGL2_MODULE GLSwapChain2 : public Ry::SwapChain
	{
	public:

		void BeginFrame(::GLFWwindow* Window, bool bWindowDirty) override;
		void EndFrame(::GLFWwindow* Window) override;

		bool CreateSwapChain(::GLFWwindow* ParentWindow) override;
		void DeleteSwapChain() override;

		int32 GetSwapChainWidth() const override;
		int32 GetSwapChainHeight() const override;

		RenderPass2* GetDefaultRenderPass() override;
	private:

		int32 SwapChainWidth = 0;
		int32 SwapChainHeight = 0;

	};
	
}
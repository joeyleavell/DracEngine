#pragma once

#include "Core/Core.h"
#include "VulkanSwapChain.h"
#include "vulkan/vulkan.h"
#include <vector>
#include "Interface/SwapChain.h"
#include "Interface/RenderPass.h"
#include "VulkanGen.h"

struct GLFWwindow;

constexpr uint32 MAX_FRAMES_IN_FLIGHT = 2;

namespace Ry
{

	class VulkanFrameBuffer;
	class VulkanRenderPass;
	class VulkanResourceSet;

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface);

	class FrameInFlight
	{
	public:

		FrameInFlight();
		~FrameInFlight();

		VkSemaphore ImageAvailableSemaphore;
		VkSemaphore RenderFinishedSemaphore;
		VkFence FlightFence;

		void CreateSyncPrimitives();
	};
	
	class VULKAN_MODULE VulkanSwapChain : public Ry::SwapChain
	{
	public:

		// Data pertaining to the default framebuffer description
		FrameBufferDescription DefaultFramebufferDesc;
		int32 DefaultColorAttachment;
		int32 DefaultDepthAttachment;

		VkSwapchainKHR SwapChain; // The swap chain that we created
		Ry::ArrayList<VkImage> SwapChainImages; // The swap chain images we retrieved
		VkFormat SwapChainImageFormat;
		VkExtent2D SwapChainExtent;
		Ry::ArrayList<VkImageView> SwapChainImageViews;

		// For storing depth image
		VkImage DepthImage;
		VkDeviceMemory DepthImageMemory;
		VkImageView DepthImageView;

		VulkanFrameBuffer* Framebuffer;

		VulkanSwapChain();

		// Swapchain interface functions
		void BeginFrame(::GLFWwindow* Window, bool bWindowDirty) override;
		void EndFrame(::GLFWwindow* Window) override;
		bool CreateSwapChain(::GLFWwindow* ParentWindow) override;
		void DeleteSwapChain() override;

		int32 GetSwapChainWidth() const override;
		int32 GetSwapChainHeight() const override;
		RenderPass* GetDefaultRenderPass() override;
		FrameBuffer* GetDefaultFrameBuffer() override;

		int32 GetSwapchainVersion();
		void SubmitBuffer(const VkCommandBuffer& BufferToSubmit);
		uint32 GetCurrentImageIndex();

		VkDescriptorPool DescriptorPool;

		/** A list of uniform buffers to update whenever a new frame is acquired */
		Ry::ArrayList<VulkanResourceSet*> ResourceSets;

	private:

		void Recreate(::GLFWwindow* ParentWindow);

		bool CreateVulkanSwapchain(::GLFWwindow* ParentWindow);
		bool CreateSurface(::GLFWwindow* ParentWindow);
		bool CreateImageViews();
		bool CreateFramebuffers();
		void CreateDefaultRenderpass();
		bool CreateDescriptorPool();
		void CreateFramesInFlight();

		void DeleteImageViews();
		void DeleteRenderPass();
		void DeleteDescriptorPool();
		void DeleteFramesInFlight();
		void DeleteSurface();
		void DeleteDepthResources();

		uint32 SwapChainVersion;

		/** A list of primary command buffers to batch */
		Ry::ArrayList<VkCommandBuffer> BuffersToSubmit;

		int32 CurrentFrame;
		Ry::ArrayList<FrameInFlight*> FramesInFlight;
		Ry::ArrayList<VkFence> ImagesInFlight;

		VulkanRenderPass* DefaultRenderPass = nullptr;

		bool bInFrame = false;
		bool bNeedsRecreationAfterPresent = false;
		uint32 AcquiredImageIndex;

		VkSurfaceKHR Surface;

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities, int32 IdealWidth, int32 IdealHeight);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats);

		// Standard Vulkan Acquire->Submit->Present flow
		void AcquireImage(::GLFWwindow* Window, bool bWindowDirty);
		void SubmitCommands();
		void PresentImage(::GLFWwindow* Window);
		
		bool CreateDepthResources();
	};

	extern VulkanSwapChain* GCurrentSwapChain;
	
}

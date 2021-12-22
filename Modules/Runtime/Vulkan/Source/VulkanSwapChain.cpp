#include "VulkanSwapChain.h"
#include "VulkanCommon.h"
#include "Core/Globals.h"
#include "VulkanContext.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanResources.h"

namespace Ry
{

	VulkanSwapChain* GCurrentSwapChain = nullptr;

	VulkanSwapChain::VulkanSwapChain()
	{
		GCurrentSwapChain = nullptr;
		this->CurrentFrame = 0;

		// Keeps track of how many times the swap chain has changed
		this->SwapChainVersion = 0;

		this->Framebuffer = nullptr;

		// Create the default framebuffer description
		DefaultColorAttachment = DefaultFramebufferDesc.AddSwapChainColorAttachment(this);
		DefaultDepthAttachment = DefaultFramebufferDesc.AddSwapChainDepthAttachment(this);

	}

	void VulkanSwapChain::BeginFrame(::GLFWwindow* Window, bool bWindowDirty)
	{
		GCurrentSwapChain = this;

		//if (bWindowDirty)
		//	Recreate(Window);

		// Wait for the current frame to finish before continuing
		// This ensures we're not spamming the GPU with more submits than necessary
		//
		FrameInFlight* CurFrame = FramesInFlight[CurrentFrame];
		vkWaitForFences(GVulkanContext->GetLogicalDevice(), 1, &CurFrame->FlightFence, VK_TRUE, UINT64_MAX);

		// todo: cleanup one time command buffers/resources here?

		// Clear out the command buffers - we have to call submit each frame
		// This allows for consistency among rendering APIs
		BuffersToSubmit.SoftClear();

		//std::cout << "window dirty: " << bWindowDirty << std::endl;

		// Acquire the next image for this frame
		AcquireImage(Window, bWindowDirty);

		bInFrame = true;

		// If this frame is in use, wait on its fence so that we don't submit to this image until the previous one is complete
		// todo: is this correct?
		if (ImagesInFlight[AcquiredImageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(GVulkanContext->GetLogicalDevice(), 1, &ImagesInFlight[AcquiredImageIndex], VK_TRUE, UINT64_MAX);
		}

		// Assign the current images fence to the one specified by the frame in flight
		ImagesInFlight[AcquiredImageIndex] = CurFrame->FlightFence;
	}

	void VulkanSwapChain::EndFrame(::GLFWwindow* Window)
	{
		// todo: cleanup?
		// Flush the uniform buffers
		// todo: optimize? This should only update resource sets that change each frame
		for(VulkanResourceSet* ResSet : ResourceSets)
		{
			ResSet->FlushBuffer(AcquiredImageIndex);
		}

		SubmitCommands();
		PresentImage(Window);

		if(bNeedsRecreationAfterPresent)
		{
			Recreate(Window);
			bNeedsRecreationAfterPresent = false;
		}

		// Keep current frame index < MAX_FRAMES_IN_FLIGHT and >= 0
		CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		bInFrame = false;
	}

	bool VulkanSwapChain::CreateSwapChain(::GLFWwindow* ParentWindow)
	{

		CreateSurface(ParentWindow);		
		CreateVulkanSwapchain(ParentWindow);
		CreateImageViews();
		// Create synchronization resources associated with CPU-GPU synchronization
		CreateFramesInFlight();
		// Create the depth resources
		CreateDepthResources();
		// Create the default render pass for the swap chain
		CreateDefaultRenderpass();
		CreateFramebuffers();
		// Create the descriptor pool
		CreateDescriptorPool();

		return true;
	}

	void VulkanSwapChain::DeleteSwapChain()
	{
		vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());

		DeleteDepthResources();

		DeleteImageViews();

		//DeleteFramebuffers();

		DeleteRenderPass();

		DeleteDescriptorPool();

		DeleteFramesInFlight();

		vkDestroySwapchainKHR(GVulkanContext->GetLogicalDevice(), SwapChain, nullptr);

		DeleteSurface();
	}

	int32 VulkanSwapChain::GetSwapChainWidth() const
	{
		return SwapChainExtent.width;
	}

	int32 VulkanSwapChain::GetSwapChainHeight() const
	{
		return SwapChainExtent.height;
	}

	bool VulkanSwapChain::CreateDescriptorPool()
	{
		// Todo: need to determine how big of a descriptor pool to allocate and keep allocations within this range
		Ry::ArrayList<VkDescriptorPoolSize> PoolSizes;

		int32 MaxValues = 1000;

		// todo: Figure out best max values
		VkDescriptorPoolSize UniformPoolSize{};
		UniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		UniformPoolSize.descriptorCount = static_cast<uint32_t>(SwapChainImages.GetSize()) * MaxValues;

		VkDescriptorPoolSize CombinedImageSamplerSize{};
		CombinedImageSamplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		CombinedImageSamplerSize.descriptorCount = static_cast<uint32_t>(SwapChainImages.GetSize()) * MaxValues;

		PoolSizes.Add(UniformPoolSize);
		PoolSizes.Add(CombinedImageSamplerSize);

		VkDescriptorPoolCreateInfo PoolInfo{};
		PoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		PoolInfo.poolSizeCount = PoolSizes.GetSize();
		PoolInfo.pPoolSizes = PoolSizes.GetData();
		PoolInfo.maxSets = static_cast<uint32_t>(SwapChainImages.GetSize()) * MaxValues;
		PoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		if (vkCreateDescriptorPool(GVulkanContext->GetLogicalDevice(), &PoolInfo, nullptr, &DescriptorPool) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create descriptor pool");
			return false;
		}

		return true;
	}

	void VulkanSwapChain::CreateFramesInFlight()
	{
		for(uint32 Index = 0; Index < MAX_FRAMES_IN_FLIGHT; Index++)
		{
			FramesInFlight.Add(new FrameInFlight);
		}

		for(int32 SwapChainImage = 0; SwapChainImage < SwapChainImages.GetSize(); SwapChainImage++)
		{
			ImagesInFlight.Add(VK_NULL_HANDLE);
		}
	}

	void VulkanSwapChain::DeleteImageViews()
	{
		for(uint32 Rp = 0; Rp < SwapChainImageViews.GetSize(); Rp++)
		{
			vkDestroyImageView(GVulkanContext->GetLogicalDevice(), SwapChainImageViews[Rp], nullptr);
		}
	}

	void VulkanSwapChain::DeleteRenderPass()
	{
		DefaultRenderPass->DeleteRenderPass();
		delete DefaultRenderPass;
	}

	void VulkanSwapChain::DeleteDescriptorPool()
	{
		vkDestroyDescriptorPool(GVulkanContext->GetLogicalDevice(), DescriptorPool, nullptr);
	}

	void VulkanSwapChain::DeleteFramesInFlight()
	{
		// Cleanup frames in flight
		for (int32 Frame = 0; Frame < MAX_FRAMES_IN_FLIGHT; Frame++)
		{
			delete FramesInFlight[Frame];
		}

		FramesInFlight.Clear();
		ImagesInFlight.Clear();
	}

	void VulkanSwapChain::DeleteSurface()
	{
		vkDestroySurfaceKHR(GVulkanContext->GetInstance(), Surface, nullptr);
	}

	void VulkanSwapChain::DeleteDepthResources()
	{
		vkDestroyImageView(GVulkanContext->GetLogicalDevice(), DepthImageView, nullptr);

		vkFreeMemory(GVulkanContext->GetLogicalDevice(), DepthImageMemory, nullptr);
		vkDestroyImage(GVulkanContext->GetLogicalDevice(), DepthImage, nullptr);
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities, int32 IdealWidth, int32 IdealHeight)
	{
		if (Capabilities.currentExtent.width != UINT32_MAX)
		{
			// Use the already determined extent			
			return Capabilities.currentExtent;
		}
		else
		{

			VkExtent2D IdealExtent =
			{
				static_cast<uint32_t>(IdealWidth),
				static_cast<uint32_t>(IdealHeight)
			};

			if (IdealExtent.width < Capabilities.minImageExtent.width)
				IdealExtent.width = Capabilities.minImageExtent.width;

			if (IdealExtent.width > Capabilities.maxImageExtent.width)
				IdealExtent.width = Capabilities.maxImageExtent.width;

			if (IdealExtent.height < Capabilities.minImageExtent.height)
				IdealExtent.height = Capabilities.minImageExtent.height;

			if (IdealExtent.height > Capabilities.maxImageExtent.height)
				IdealExtent.height = Capabilities.maxImageExtent.height;


			return IdealExtent;
		}
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes)
	{
		for (const auto& AvailablePresentMode : AvailablePresentModes)
		{
			if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return AvailablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
	{
		for (const auto& AvailableFormat : AvailableFormats)
		{
			if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return AvailableFormat;
			}
		}

		// Settle on the first format
		return AvailableFormats[0];
	}

	bool VulkanSwapChain::CreateDepthResources()
	{
		VkFormat DepthFormat;

		if (!FindDepthFormat(DepthFormat))
		{
			Ry::Log->LogError("Failed to find a depth format");
			return false;
		}

		if(!CreateImage(
			SwapChainExtent.width,
			SwapChainExtent.height,
			DepthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			DepthImage,
			DepthImageMemory
		))
		{
			Ry::Log->LogError("Failed to create a depth image");
			return false;
		}

		if (!CreateImageView(
			DepthImageView,
			DepthImage,
			DepthFormat,
			VK_IMAGE_ASPECT_DEPTH_BIT
		))
		{
			Ry::Log->LogError("Failed to create an image view for the depth image");
			return false;
		}

		// We don't need to transition the depth image here since the render pass will take care of that for us
		

		return true;
	}

	void VulkanSwapChain::AcquireImage(::GLFWwindow* Window, bool bWindowDirty)
	{
		FrameInFlight* CurFrame = FramesInFlight[CurrentFrame];

		// Acquire the next image for rendering to
		VkResult AcquireImageResult = vkAcquireNextImageKHR(GVulkanContext->GetLogicalDevice(), SwapChain, UINT64_MAX, CurFrame->ImageAvailableSemaphore, VK_NULL_HANDLE, &AcquiredImageIndex);

		if (AcquireImageResult == VK_ERROR_OUT_OF_DATE_KHR || AcquireImageResult == VK_SUBOPTIMAL_KHR || bWindowDirty)
		{
			bNeedsRecreationAfterPresent = true;
		}
		else if (AcquireImageResult != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to acquire swap chain image");
		}
	}

	void VulkanSwapChain::SubmitCommands()
	{
		FrameInFlight* CurFrame = FramesInFlight[CurrentFrame];

		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkPipelineStageFlags WaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = &CurFrame->ImageAvailableSemaphore;
		SubmitInfo.pWaitDstStageMask = WaitStages;
		SubmitInfo.commandBufferCount = BuffersToSubmit.GetSize();
		SubmitInfo.pCommandBuffers = BuffersToSubmit.GetData();

		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = &CurFrame->RenderFinishedSemaphore;

		vkResetFences(GVulkanContext->GetLogicalDevice(), 1, &CurFrame->FlightFence);

		// Use the frame in flight's fence
		if (vkQueueSubmit(GVulkanContext->GetGraphicsQueue(), 1, &SubmitInfo, CurFrame->FlightFence) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to submit draw command buffer!");
		}
	}

	void VulkanSwapChain::PresentImage(::GLFWwindow* Window)
	{
		FrameInFlight* CurFrame = FramesInFlight[CurrentFrame];

		VkPresentInfoKHR PresentInfo{};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = &CurFrame->RenderFinishedSemaphore; // Wait on rendering to complete before presenting the image

		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = &SwapChain;
		PresentInfo.pImageIndices = &AcquiredImageIndex;
		PresentInfo.pResults = nullptr; // Optional
		VkResult PresentResult = vkQueuePresentKHR(GVulkanContext->GetPresentQueue(), &PresentInfo);

		if (PresentResult == VK_ERROR_OUT_OF_DATE_KHR || PresentResult == VK_SUBOPTIMAL_KHR)
		{
			//std::cout << "out of date present" << std::endl;
			bNeedsRecreationAfterPresent = true;
		}
		else if (PresentResult != VK_SUCCESS) 
		{
			Ry::Log->LogError("Failed to present image to Vulkan pipeline");
		}
		
	}

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface)
	{
		SwapChainSupportDetails Details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Details.Capabilities);

		uint32_t FormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);

		if (FormatCount != 0)
		{
			Details.Formats.resize(FormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Details.Formats.data());
		}

		uint32_t PresentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, nullptr);

		if (PresentModeCount != 0)
		{
			Details.PresentModes.resize(PresentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Details.PresentModes.data());
		}

		return Details;
	}

	FrameInFlight::FrameInFlight()
	{
		CreateSyncPrimitives();
	}

	FrameInFlight::~FrameInFlight()
	{
		vkDestroySemaphore(GVulkanContext->GetLogicalDevice(), RenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(GVulkanContext->GetLogicalDevice(), ImageAvailableSemaphore, nullptr);

		vkDestroyFence(GVulkanContext->GetLogicalDevice(), FlightFence, nullptr);
	}

	void FrameInFlight::CreateSyncPrimitives()
	{
		// Create GPU-GPU synchronization semaphores
		VkSemaphoreCreateInfo SemaphoreInfo{};
		SemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(GVulkanContext->GetLogicalDevice(), &SemaphoreInfo, nullptr, &ImageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(GVulkanContext->GetLogicalDevice(), &SemaphoreInfo, nullptr, &RenderFinishedSemaphore) != VK_SUCCESS) {

			Ry::Log->LogError("Failed to create Vulkan semaphores!");
		}

		// Create CPU-GPU synchronization fence
		VkFenceCreateInfo FenceInfo{};
		FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Create this fence in the signaled state

		if (vkCreateFence(GVulkanContext->GetLogicalDevice(), &FenceInfo, nullptr, &FlightFence) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create Vulkan fence, will not be able to synchronize CPU with GPU");
		}

	}

	void VulkanSwapChain::CreateDefaultRenderpass()
	{
		DefaultRenderPass = new VulkanRenderPass;
		DefaultRenderPass->SetFramebufferDescription(DefaultFramebufferDesc);
		
		int32 MainPass = DefaultRenderPass->CreateSubpass();
		DefaultRenderPass->AddSubpassAttachment(MainPass, DefaultColorAttachment);
		DefaultRenderPass->AddSubpassAttachment(MainPass, DefaultDepthAttachment);
		DefaultRenderPass->CreateRenderPass();
	}

	RenderPass* VulkanSwapChain::GetDefaultRenderPass()
	{
		return DefaultRenderPass;
	}

	FrameBuffer* VulkanSwapChain::GetDefaultFrameBuffer()
	{
		return Framebuffer;
	}

	int32 VulkanSwapChain::GetSwapchainVersion()
	{
		return SwapChainVersion;
	}

	void VulkanSwapChain::SubmitBuffer(const VkCommandBuffer& BufferToSubmit)
	{
		BuffersToSubmit.Add(BufferToSubmit);
	}

	uint32 VulkanSwapChain::GetCurrentImageIndex()
	{
		if (!bInFrame)
			return -1;
		
		return AcquiredImageIndex;
	}

	void VulkanSwapChain::Recreate(::GLFWwindow* ParentWindow)
	{
		// Wait on device idle
		vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());

		// Delete old resources
		{
			DeleteDepthResources();
			DeleteImageViews();
			//DeleteFramebuffers();
			DeleteRenderPass();
			vkDestroySwapchainKHR(GVulkanContext->GetLogicalDevice(), SwapChain, nullptr);
		}

		// Create new resources
		{
			CreateVulkanSwapchain(ParentWindow);
			CreateImageViews();
			CreateDepthResources();
			CreateDefaultRenderpass();
			CreateFramebuffers();
		}

		// Increment swap chain version
		this->SwapChainVersion++;

		OnSwapChainDirty.Broadcast();
	}

	bool VulkanSwapChain::CreateVulkanSwapchain(::GLFWwindow* ParentWindow)
	{
		int32 FramebufferWidth, FramebufferHeight;
		glfwGetFramebufferSize(ParentWindow, &FramebufferWidth, &FramebufferHeight);

		// Can't create swapchain until we have a size > 0
		while (FramebufferWidth == 0 || FramebufferHeight == 0)
		{
			glfwGetFramebufferSize(ParentWindow, &FramebufferWidth, &FramebufferHeight);
			glfwWaitEvents();
		}

		SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(GVulkanContext->GetPhysicalDevice(), Surface);
		VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainSupport.Formats);
		VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainSupport.PresentModes);
		VkExtent2D Extent = ChooseSwapExtent(SwapChainSupport.Capabilities, FramebufferWidth, FramebufferHeight);

		// Request one more image here so we don't have to wait on driver
		uint32_t ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;

		SwapChainImageFormat = SurfaceFormat.format;
		SwapChainExtent = Extent;

		// Zero means there is no maximum
		if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount)
		{
			ImageCount = SwapChainSupport.Capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		CreateInfo.surface = Surface;
		CreateInfo.minImageCount = ImageCount;
		CreateInfo.imageFormat = SurfaceFormat.format;
		CreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
		CreateInfo.imageExtent = Extent;
		CreateInfo.imageArrayLayers = 1; // This is always one unless we're developing for stereoscopic (VR)
		CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilies& Indices = GVulkanContext->GetQueueIndices();
		uint32_t QueueFamilyIndices[] = { (uint32) Indices.GraphicsFamily, (uint32) Indices.PresentFamily };

		// If our queues are not the same, make the sharing mode concurrent
		if (Indices.GraphicsFamily != Indices.PresentFamily)
		{
			CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			CreateInfo.queueFamilyIndexCount = 2;
			CreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
		}
		else
		{
			CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			CreateInfo.queueFamilyIndexCount = 0; // Optional
			CreateInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		CreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
		CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		CreateInfo.presentMode = PresentMode;
		CreateInfo.clipped = VK_TRUE;
		CreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult SwapChainCreateResult = vkCreateSwapchainKHR(GVulkanContext->GetLogicalDevice(), &CreateInfo, nullptr, &SwapChain);
		if (SwapChainCreateResult != VK_SUCCESS)
		{
			Ry::Log->LogErrorf("Failed to create Vulkan swap chain: %d", (int32)SwapChainCreateResult);
			return false;
		}

	//	Ry::Log->Log("Created Vulkan swap chain");

		vkGetSwapchainImagesKHR(GVulkanContext->GetLogicalDevice(), SwapChain, &ImageCount, nullptr);
		SwapChainImages.SetSize(ImageCount);

		vkGetSwapchainImagesKHR(GVulkanContext->GetLogicalDevice(), SwapChain, &ImageCount, SwapChainImages.GetData());

		return true;
	}

	bool VulkanSwapChain::CreateSurface(::GLFWwindow* ParentWindow)
	{
		// Create the surface
		if (glfwCreateWindowSurface(GVulkanContext->GetInstance(), ParentWindow, nullptr, &Surface) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create surface for Vulkan");
			return false;
		}

		// Ensure the created surface is still compatible with the physical device
		VkBool32 PresentSupport;
		vkGetPhysicalDeviceSurfaceSupportKHR(GVulkanContext->GetPhysicalDevice(), GVulkanContext->GetQueueIndices().PresentFamily, Surface, &PresentSupport);

		if (!PresentSupport)
		{
			Ry::Log->LogError("Created surface was not compatible with physical device and present family");
			return false;
		}

		return true;
	}

	bool VulkanSwapChain::CreateImageViews()
	{
		SwapChainImageViews.SetSize(SwapChainImages.GetSize());

		for (size_t i = 0; i < SwapChainImages.GetSize(); i++)
		{
			VkImageViewCreateInfo CreateInfo{};
			CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			CreateInfo.image = SwapChainImages[i];

			CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			CreateInfo.format = SwapChainImageFormat;

			CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			CreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			CreateInfo.subresourceRange.baseMipLevel = 0;
			CreateInfo.subresourceRange.levelCount = 1;
			CreateInfo.subresourceRange.baseArrayLayer = 0;
			CreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(GVulkanContext->GetLogicalDevice(), &CreateInfo, nullptr, &SwapChainImageViews[i]) != VK_SUCCESS)
			{
				Ry::Log->LogError("Failed to create Vulkan image view");
				return false;
			}
		}

		return true;
	}

	bool VulkanSwapChain::CreateFramebuffers()
	{
		if(!Framebuffer)
		{
			Framebuffer = new VulkanFrameBuffer(SwapChainExtent.width, SwapChainExtent.height, DefaultRenderPass, &DefaultFramebufferDesc);
		}
		else
		{
			Framebuffer->Recreate(SwapChainExtent.width, SwapChainExtent.height, DefaultRenderPass);
		}

		//SwapChainFramebuffers.resize(SwapChainImages.size());

		// Create a framebuffer for each image view
		/*for (size_t i = 0; i < SwapChainImageViews.GetSize(); i++)
		{
			// TODO: use new framebuffer attachment spec system
			VulkanFrameBuffer* NewFBO = new VulkanFrameBuffer(SwapChainExtent.width, SwapChainExtent.height, nullptr);
			NewFBO->AddAttachment(SwapChainImageViews[i]);
			NewFBO->AddAttachment(DepthImageView);
			NewFBO->CreateFrameBuffer(DefaultRenderPass->GetRenderPass());
			SwapChainFramebuffers.Add(NewFBO);

			// VkImageView Attachments[] = {
			// 	SwapChainImageViews[i]
			// };
			//
			// VkFramebufferCreateInfo FramebufferInfo{};
			// FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			// FramebufferInfo.renderPass = RenderPass;
			// FramebufferInfo.attachmentCount = 1;
			// FramebufferInfo.pAttachments = Attachments;
			// FramebufferInfo.width = SwapChainExtent.width;
			// FramebufferInfo.height = SwapChainExtent.height;
			// FramebufferInfo.layers = 1;
			//
			// if (vkCreateFramebuffer(LogDevice, &FramebufferInfo, nullptr, &SwapChainFramebuffers[i]) != VK_SUCCESS)
			// {
			// 	Ry::Log->LogError("Failed to create Vulkan framebuffer");
			// 	return false;
			// }
		}*/

		return true;
	}

	
}

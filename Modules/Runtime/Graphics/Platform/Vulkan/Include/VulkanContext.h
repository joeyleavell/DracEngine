#pragma once

#define VULKAN_VALIDATION 1

#include "Core/Core.h"
#include "VulkanGen.h"
#include <vector>
#include "Context.h"
#include "vulkan/vulkan.h"
#include "VulkanCommandBuffer.h"

struct GLFWwindow;

namespace Ry
{

	class VulkanSwapChain;
	class VulkanPipeline;
	class VulkanRenderPass;
	class VulkanCommandBuffer;
	class SwapChain;

	class VULKAN_MODULE VulkanContext : public Ry::Context
	{
	private:

		// Nothing should create a vulkan context besides the API initialization function
		VulkanContext() = default;
		~VulkanContext() = default;

	public:

		friend bool InitVulkanAPI();
		friend void ShutdownVulkanAPI();

		bool PreWindowCreation(::GLFWwindow* Window) override;

		bool InitContext() override;
		void CleanupContext() override;
		//bool RecreateSwapChain(::GLFWwindow* Window);

		VkPhysicalDevice GetPhysicalDevice() const;
		VkDevice GetLogicalDevice() const;
		VkInstance GetInstance() const;
		VkQueue GetGraphicsQueue() const;
		VkQueue GetPresentQueue() const;
		VkCommandPool GetCommandPool() const;
		QueueFamilies& GetQueueIndices();

	private:

		QueueFamilies QueueIndices;
		Ry::ArrayList<Ry::String> RequiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		Ry::ArrayList<Ry::String> RequiredValidationLayers = { "VK_LAYER_KHRONOS_validation" };
		Ry::ArrayList<Ry::String> RequiredInstanceExtensions;

		// todo: The fields grouped below are probably the only things that should be associated with the vulkan context
		VkQueue GraphicsQueue;
		VkQueue PresentQueue;
		VkInstance VulkanInstance;
		VkDebugUtilsMessengerEXT DebugMessenger;
		VkCommandPool CommandPool;
		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
		VkDevice LogicalDevice;

		::GLFWwindow* DummyWindow;
		VkSurfaceKHR DummySurface;
		
		bool CreateDummySurface();
		bool CreateVulkanInstance();
		bool PickPhysicalDevice();
		bool CreateLogicalDevice();
		bool CreateCommandPool();
		void DeleteDummySurface();
		bool IsDeviceSuitable(VkPhysicalDevice PhysicalDev);

#if !defined(RYBUILD_CONFIG_SHIPPING) && defined(VULKAN_VALIDATION)
		char** ValidationLayers = nullptr;
		int32 NumValidationLayers = 0;
		bool SetupVulkanDebug();
#endif

	};

	extern VULKAN_MODULE VulkanContext* GVulkanContext;

	
}

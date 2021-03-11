#include "VulkanContext.h"
#include "Core/Globals.h"
#include <set>
#include "VulkanSwapChain.h"
#include "VulkanVertexArray.h"
#include "VulkanShader.h"
#include "VulkanCommon.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

namespace Ry
{

	/**
	 *
	 * The global Vulkan context. This is only valid after the first window in the program is created.
	 *
	 * This is because we need information about the window before determining physical device selection and thus logical device creation.
	 */
	Ry::VulkanContext* GVulkanContext = nullptr;

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	bool VulkanContext::PreWindowCreation(::GLFWwindow* Window)
	{

		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // todo: disable window resizing for now, fix later
		
		return true;
	}

	bool VulkanContext::InitContext()
	{
		if (!glfwVulkanSupported())
		{
			Ry::Log->LogError("Vulkan is not supported");
			return false;
		}
		
		if (!CreateVulkanInstance())
		{
			Ry::Log->LogError("Failed to create Vulkan instance");
		}
		else
		{
			Ry::Log->Log("Created Vulkan instance");
		}

		if(!SetupVulkanDebug())
		{
			Ry::Log->LogError("Failed to setup Vulkan debug");
			return false;
		}

		if (!CreateDummySurface())
		{
			return false;
		}

		if (!PickPhysicalDevice())
		{
			return false;
		}

		if (!CreateLogicalDevice())
		{
			return false;
		}

		if (!CreateCommandPool())
		{
			return false;
		}

		// Delete the dummy surface now that we've used it to find proper present queue family
		DeleteDummySurface();
		
		return true;
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanContext::CleanupContext()
	{

#ifndef RYBUILD_CONFIG_SHIPPING
		DestroyDebugUtilsMessengerEXT(VulkanInstance, DebugMessenger, nullptr);
#endif
		
		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);
				
		vkDestroyDevice(LogicalDevice, nullptr);
		vkDestroyInstance(VulkanInstance, nullptr);
	}

	VkPhysicalDevice VulkanContext::GetPhysicalDevice() const
	{
		return PhysicalDevice;
	}

	VkDevice VulkanContext::GetLogicalDevice() const
	{
		return LogicalDevice;
	}

	VkInstance VulkanContext::GetInstance() const
	{
		return VulkanInstance;
	}

	VkQueue VulkanContext::GetGraphicsQueue() const
	{
		return GraphicsQueue;
	}

	VkQueue VulkanContext::GetPresentQueue() const
	{
		return PresentQueue;
	}

	VkCommandPool VulkanContext::GetCommandPool() const
	{
		return CommandPool;
	}

	QueueFamilies& VulkanContext::GetQueueIndices()
	{
		return QueueIndices;
	}

	bool VulkanContext::IsDeviceSuitable(VkPhysicalDevice PhysicalDev)
	{
		// VkPhysicalDeviceFeatures DeviceFeatures;
		// vkGetPhysicalDeviceFeatures(PhysicalDev, &DeviceFeatures);

		if (!CheckDeviceExtensionSupport(PhysicalDev, RequiredDeviceExtensions))
		{
			return false;
		}

		Ry::SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(PhysicalDev, DummySurface);

		if(SwapChainSupport.Formats.empty())
		{
			return false;
		}

		if(SwapChainSupport.PresentModes.empty())
		{
			return false;
		}
				
		return true;
	}

	// bool VulkanContext::RecreateSwapChain(::GLFWwindow* Window)
	// {
	// 	int Width = 0, Height = 0;
	// 	glfwGetFramebufferSize(Window, &Width, &Height);
	// 	while (Width == 0 || Height == 0)
	// 	{
	// 		glfwGetFramebufferSize(Window, &Width, &Height);
	// 		glfwWaitEvents();
	// 	}
	//
	// 	vkDeviceWaitIdle(LogicalDevice);
	//
	// 	CleanupSwapChain();
	//
	// 	SwapChain->CreateSwapChain(PhysicalDevice, LogicalDevice, Surface, Width, Height);
	//
	// 	return true;
	// }

	bool VulkanContext::CreateDummySurface()
	{
		// Create dummy window and surface so we can query for a proper physical device
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		DummyWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);
		
		VkResult Result = glfwCreateWindowSurface(VulkanInstance, DummyWindow, nullptr, &DummySurface);
		if (Result != VK_SUCCESS)
		{
			Ry::Log->LogErrorf("Vulkan error creating dummy surface: %d", (int32)Result);
			return false;
		}

		return true;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

#ifndef RYBUILD_CONFIG_SHIPPING
	bool VulkanContext::SetupVulkanDebug()
	{
		VkDebugUtilsMessengerCreateInfoEXT CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		CreateInfo.pfnUserCallback = &debugCallback;
		CreateInfo.pUserData = nullptr; // Optional
		
		if (CreateDebugUtilsMessengerEXT(VulkanInstance, &CreateInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
		{
			return false;
		}
		
		return true;
	}
#endif

	bool VulkanContext::PickPhysicalDevice()
	{

		uint32_t DeviceCount = 0;
		vkEnumeratePhysicalDevices(VulkanInstance, &DeviceCount, nullptr);

		if (DeviceCount == 0)
		{
			Ry::Log->LogError("No GPUs with Vulkan support");
			return false;
		}

		std::vector<VkPhysicalDevice> Devices(DeviceCount);
		vkEnumeratePhysicalDevices(VulkanInstance, &DeviceCount, Devices.data());

		for (const VkPhysicalDevice& Device : Devices)
		{
			QueueFamilies Queues;
			Ry::FindQueueFamiliesForDevice(Queues, Device, DummySurface);

			if(Queues.PresentFamily < 0 || Queues.GraphicsFamily < 0)
			{
				continue;
			}
			
			if (IsDeviceSuitable(Device))
			{
				PhysicalDevice = Device;
				QueueIndices = Queues;

				// todo: in the future, be more intelligent about the physical device we select
				// if(DeviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && DeviceFeatures.geometryShader)
				// {
				// 	PhysicalDevice = Device;
				// }


				break;
			}

		}

		if (PhysicalDevice == VK_NULL_HANDLE)
		{
			Ry::Log->LogError("Failed to find suitable physical Vulkan device");
			return false;
		}
		else
		{
			VkPhysicalDeviceProperties DeviceProps;
			vkGetPhysicalDeviceProperties(PhysicalDevice, &DeviceProps);

			Ry::Log->Logf("Found suitable Vulkan device: %s", DeviceProps.deviceName);

			return true;
		}

	}

	bool VulkanContext::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
		std::set<int32> UniqueQueueFamilies = { QueueIndices.GraphicsFamily, QueueIndices.PresentFamily };

		float QueuePriority = 1.0f;
		for (int32 UniqueQueueFam : UniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo QueueCreateInfo{};
			QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QueueCreateInfo.queueFamilyIndex = UniqueQueueFam;
			QueueCreateInfo.queueCount = 1;
			QueueCreateInfo.pQueuePriorities = &QueuePriority;

			QueueCreateInfos.push_back(QueueCreateInfo);
		}

		// Specify used device features here
		VkPhysicalDeviceFeatures DeviceFeatures{};

		char** DeviceExtensionsAsCString = new char* [RequiredDeviceExtensions.GetSize()];
		for(int32 Ext = 0; Ext < RequiredDeviceExtensions.GetSize(); Ext++)
		{
			DeviceExtensionsAsCString[Ext] = *RequiredDeviceExtensions[Ext];
		}

		// Create logical device
		VkDeviceCreateInfo CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		CreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
		CreateInfo.queueCreateInfoCount = QueueCreateInfos.size();
		CreateInfo.pEnabledFeatures = &DeviceFeatures;
		CreateInfo.enabledExtensionCount = static_cast<uint32>(RequiredDeviceExtensions.GetSize());
		CreateInfo.ppEnabledExtensionNames = DeviceExtensionsAsCString;

#ifndef RYBUILD_DISTRIBUTE
		CreateInfo.enabledLayerCount = static_cast<uint32_t>(NumValidationLayers);
		CreateInfo.ppEnabledLayerNames = ValidationLayers;
#else
		CreateInfo.enabledLayerCount = 0;
#endif

		if (vkCreateDevice(PhysicalDevice, &CreateInfo, nullptr, &LogicalDevice) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create logical Vulkan device");
			return false;
		}
		else
		{
			Ry::Log->Log("Created logical Vulkan device");
		}

		// Get our queues
		vkGetDeviceQueue(LogicalDevice, QueueIndices.GraphicsFamily, 0, &GraphicsQueue);
		vkGetDeviceQueue(LogicalDevice, QueueIndices.PresentFamily, 0, &PresentQueue);

		return true;
	}

	bool VulkanContext::CreateCommandPool()
	{
		VkCommandPoolCreateInfo PoolInfo{};
		PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		PoolInfo.queueFamilyIndex = QueueIndices.GraphicsFamily;
		PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Todo: Should this be configurable?

		if (vkCreateCommandPool(LogicalDevice, &PoolInfo, nullptr, &CommandPool) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create Vulkan command pool");
			return false;
		}
		else
		{
			Ry::Log->Log("Created Vulkan command pool");
		}

		return true;
	}

	void VulkanContext::DeleteDummySurface()
	{
		// Destroy dummy surface
		vkDestroySurfaceKHR(VulkanInstance, DummySurface, nullptr);

		// Destroy dummy window
		glfwDestroyWindow(DummyWindow);
	}

	bool VulkanContext::CreateVulkanInstance()
	{
		Ry::ArrayList<Ry::String> SupportedExtensions;

#ifndef RYBUILD_DISTRIBUTE
		if (!CheckValidationLayerSupport(RequiredValidationLayers))
		{
			Ry::Log->LogError("Requested validation layers not available");
			return false;
		}
#endif

		// Determine required vulkan extensions
		{
			uint32_t GLFWExtCount = 0;
			const char** GLFWExtensions = glfwGetRequiredInstanceExtensions(&GLFWExtCount);
			for (int32 RequiredExt = 0; RequiredExt < GLFWExtCount; RequiredExt++)
			{
				const char* GLFWExt = GLFWExtensions[RequiredExt];
				RequiredInstanceExtensions.Add(GLFWExt);
			}

#ifndef RYBUILD_CONFIG_SHIPPING
			RequiredInstanceExtensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		}

		// Query the supported Vulkan extensions
		{
			uint32_t SupportedExtCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &SupportedExtCount, nullptr);

			VkExtensionProperties* SupportedExtensionsArray = new VkExtensionProperties[SupportedExtCount];
			vkEnumerateInstanceExtensionProperties(nullptr, &SupportedExtCount, SupportedExtensionsArray);

			Ry::Log->Log("Available extensions:");
			for(int32 Ext = 0; Ext < SupportedExtCount; Ext++)
			{
				SupportedExtensions.Add(SupportedExtensionsArray[Ext].extensionName);
				Ry::Log->Logf("\t%s", SupportedExtensionsArray[Ext].extensionName);
			}
			
		}

		// Check to make sure all required extensions are supported
		for (const Ry::String& RequiredExt : RequiredInstanceExtensions)
		{
			if (!SupportedExtensions.Contains(RequiredExt))
			{
				Ry::Log->LogErrorf("Required extension %s not supported!", *RequiredExt);
				return false;
			}
		}

		VkApplicationInfo ApplicationInfo{};
		ApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		ApplicationInfo.pApplicationName = "Hello Triangle";
		ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		ApplicationInfo.pEngineName = "AryzeEngine";
		ApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		ApplicationInfo.apiVersion = VK_API_VERSION_1_1;

		VkInstanceCreateInfo CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		CreateInfo.pApplicationInfo = &ApplicationInfo;

		char** InstanceExtCString = new char* [RequiredInstanceExtensions.GetSize()];
		for(int32 Ext = 0; Ext < RequiredInstanceExtensions.GetSize(); Ext++)
		{
			InstanceExtCString[Ext] = *RequiredInstanceExtensions[Ext];
		}
		
		CreateInfo.enabledExtensionCount = RequiredInstanceExtensions.GetSize();
		CreateInfo.ppEnabledExtensionNames = InstanceExtCString;

#ifndef RYBUILD_DISTRIBUTE
		// Create the c string array of validation layers as these will also be used later in device creation
		ValidationLayers = new char* [RequiredValidationLayers.GetSize()];
		NumValidationLayers = RequiredValidationLayers.GetSize();
		for (int32 ValLayerIndex = 0; ValLayerIndex < RequiredValidationLayers.GetSize(); ValLayerIndex++)
		{
			Ry::String ValLayer = RequiredValidationLayers[ValLayerIndex];
			ValidationLayers[ValLayerIndex] = new char[ValLayer.getSize() + 1];
			MemCpy(ValidationLayers[ValLayerIndex], ValLayer.getSize() + 1, *ValLayer, ValLayer.getSize() + 1);
		}

		CreateInfo.enabledLayerCount = RequiredValidationLayers.GetSize();
		CreateInfo.ppEnabledLayerNames = ValidationLayers;
#else
		CreateInfo.enabledLayerCount = 0;
#endif

		bool bSuccess = vkCreateInstance(&CreateInfo, nullptr, &VulkanInstance) == VK_SUCCESS;

		delete[] InstanceExtCString;

		return bSuccess;
	}
}

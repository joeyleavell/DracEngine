#pragma once

#include "VulkanCommon.h"
#include "Interface/RenderPass.h"
#include "VulkanGen.h"
#include "Data/Map.h"

namespace Ry
{

	class VULKAN_MODULE VulkanRenderPass : public Ry::RenderPass
	{
	public:

		VkRenderPass GetRenderPass();

		bool CreateRenderPass() override;
		void DeleteRenderPass() override;


	private:

		VkRenderPass RenderPass;
	};
	
}
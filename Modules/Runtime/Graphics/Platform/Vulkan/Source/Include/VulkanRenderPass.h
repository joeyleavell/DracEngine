#pragma once

#include "VulkanCommon.h"
#include "Interface2/RenderPass2.h"
#include "VulkanGen.h"
#include "Data/Map.h"

namespace Ry
{

	class VULKAN_MODULE VulkanRenderPass : public Ry::RenderPass2
	{
	public:

		VkRenderPass GetRenderPass();

		bool CreateRenderPass() override;
		void DeleteRenderPass() override;


	private:

		VkRenderPass RenderPass;
	};
	
}
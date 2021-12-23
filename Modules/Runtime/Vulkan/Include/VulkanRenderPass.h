#pragma once

#include "VulkanCommon.h"
#include "RenderPass.h"
#include "VulkanGen.h"

namespace Ry
{

	class VULKAN_MODULE VulkanRenderPass : public Ry::RenderPass
	{
	public:

		VkRenderPass GetRenderPass() const;

		bool CreateRenderPass() override;
		void DeleteRenderPass() override;

	private:

		VkRenderPass RenderPass;
	};
	
}
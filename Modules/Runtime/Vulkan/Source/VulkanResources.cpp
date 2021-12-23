#include "VulkanResources.h"
#include "Core/Globals.h"
#include "VulkanBuffer.h"
#include "Vector.h"
#include "VulkanSwapChain.h"
#include "VulkanContext.h"
#include "vulkan/vulkan.h"
#include "VulkanTexture.h"

namespace Ry
{

	void MappedConstantBuffer::Cleanup()
	{
		for (VulkanBuffer* UniformBuffer : VulkanBuffers)
		{
			delete UniformBuffer;
		}
	}

	VulkanResourceSet::VulkanResourceSet(const ResourceLayout* CreateInfo, SwapChain* SC):
	ResourceSet(CreateInfo, SC)
	{
		CreateBufferStorage();		
	}

	void VulkanResourceSet::CreateBuffer()
	{
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);
		assert(VkSC != nullptr);

		// Register this uniform buffer to be updated
		// todo: be able to register a uniform buffer as only updating on certain frames i.e. when the data is changed?
		// todo: could add a buffer hint usage such as dynamic, static, etc
		VkSC->ResourceSets.Add(this);

		// Check that all texture spots were bound
		for(int32 TextureBinding = 0; TextureBinding < Info->TextureBindings.GetSize(); TextureBinding++)
		{
			Ry::String IdName = Info->TextureBindings[TextureBinding]->Name;

			if(!MappedTextures.Contains(IdName))
			{
				Ry::Log->LogWarnf("Texture %s was not bound!", *IdName);
			}
		}
		
		CreateUniformBuffers(VkSC);
		CreateDescriptorSets(VkSC);
	}

	void VulkanResourceSet::DeleteBuffer()
	{
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);
		assert(VkSC != nullptr);
		VkSC->ResourceSets.Remove(this);

		Ry::OAPairIterator<Ry::String, MappedConstantBuffer*> Itr = MappedConstantBuffers.CreatePairIterator();
		while(Itr)
		{
			Itr.GetValue()->Cleanup();

			delete (Itr.GetValue());
			
			++Itr;
		}

		vkFreeDescriptorSets(GVulkanContext->GetLogicalDevice(), VkSC->DescriptorPool, DescriptorSets.GetSize(), DescriptorSets.GetData());
		
	}

	bool VulkanResourceSet::Update()
	{
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);
		int FlightIndex = VkSC->GetCurrentImageIndex();

		if(FlightIndex >= 0 && DirtyFrames.Contains(FlightIndex))
		{
			DirtyFrames.Remove(FlightIndex);

			// Update this descriptor set
			UpdateDescriptorSet(VkSC, FlightIndex);

			return true;
		}

		if(bDirtyThisFrame)
		{
			bDirtyThisFrame = false;
			return true;
		}

		return false;
	}

	void VulkanResourceSet::FlushBuffer(int32 Frame)
	{
		// Dirty the uniform frames
		if (!UniformDirtyFrames.Contains(Frame))
		{
			return;
		}
		
		UniformDirtyFrames.Remove(Frame);
		
		// Flush all constant buffers
		for(const ConstantBuffer* ConstantBuffer : Info->ConstantBuffers)
		{
			// Get the mapped buffer
			MappedConstantBuffer* Mapped = MappedConstantBuffers.Get(ConstantBuffer->Name);

			// Update the buffer data
			VulkanBuffer* UniformBuffForFrame = Mapped->VulkanBuffers[Frame];
			UniformBuffForFrame->UploadData(Mapped->HostDataBuffer, Mapped->HostBufferSize);
		}
	}

	void VulkanResourceSet::BindFrameBufferAttachment(Ry::String TextureName, const Ry::ColorAttachment* ColorAttachment)
	{
		if(const VulkanColorAttachment* Attachment = dynamic_cast<const VulkanColorAttachment*>(ColorAttachment))
		{
			BindTexture(TextureName, Attachment->ImageView, Attachment->Sampler);
		}
		else
		{
			Ry::Log->LogError("VulkanResourceSet::BindFrameBufferAttachment: Color attachment was invalid");
		}
	}

	void VulkanResourceSet::BindTexture(Ry::String TextureName, const Ry::Texture* Resource)
	{
		const VulkanTexture* VkTexture = dynamic_cast<const VulkanTexture*>(Resource);
		assert(VkTexture != nullptr);

		if(VkTexture)
		{
			BindTexture(TextureName, VkTexture->GetImageView(), VkTexture->GetSampler());
		}
	}

	void VulkanResourceSet::BindTexture(const Ry::String TextureName, VkImageView Image, VkSampler Sampler)
	{
		MappedTexture* Map = nullptr;

		if (MappedTextures.Contains(TextureName))
		{
			Map = MappedTextures.Get(TextureName);
		}
		else
		{
			Map = new MappedTexture;
			MappedTextures.Insert(TextureName, Map);
		}

		if (Map)
		{
			Map->Image = Image;
			Map->Sampler = Sampler;
		}

		// Update descriptor sets if we've already created them
		if (DescriptorSets.GetSize() > 0)
		{
			VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);
			int FlightIndex = VkSC->GetCurrentImageIndex();

			if (FlightIndex < 0)
			{
				// Device wait, not in a frame so we have to update all (not efficient)			
				vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());

				for (int32 Index = 0; Index < VkSC->SwapChainImages.GetSize(); Index++)
				{
					UpdateDescriptorSet(VkSC, Index);
				}
			}
			else
			{
				// Update this frame, mark others as dirty
				UpdateDescriptorSet(VkSC, FlightIndex);

				for (int32 Index = 0; Index < VkSC->SwapChainImages.GetSize(); Index++)
				{
					if (Index != FlightIndex && !DirtyFrames.Contains(Index))
					{
						DirtyFrames.Add(Index);
					}
				}

				bDirtyThisFrame = true;
			}

		}
	}

	void VulkanResourceSet::SetConstant(Ry::String BufferName, Ry::String Id, const void* Data)
	{
		if(!MappedConstantBuffers.Contains(BufferName))
		{
			Ry::Log->LogErrorf("Constant buffer named %s does not exist", *BufferName);
			return;
		}
		
		MappedConstantBuffer* Buff = MappedConstantBuffers.Get(BufferName);

		if(!Buff->UniformRefs.Contains(Id))
		{
			Ry::Log->LogErrorf("Uniform %s in constant buffer %s does not exist", *Id, *BufferName);
			return;
		}

		const BufferRef& Ref = Buff->UniformRefs.Get(Id);
		MemCpy(Buff->HostDataBuffer + Ref.Offset, Ref.SlotSize, Data, Ref.SlotSize);

		// Dirty the uniform frames
		VulkanSwapChain* VkSwap = dynamic_cast<VulkanSwapChain*>(Swap);
		for (int32 Frame = 0; Frame < VkSwap->SwapChainImages.GetSize(); Frame++)
		{
			if (!UniformDirtyFrames.Contains(Frame))
			{
				UniformDirtyFrames.Add(Frame);
			}
		}
	}
	
	void VulkanResourceSet::CreateUniformBuffers(VulkanSwapChain* SwapChain)
	{
		for(const ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			if (!MappedConstantBuffers.Contains(ConstBuffer->Name))
			{
				Ry::Log->LogErrorf("Mapped constant buffer not yet created for %s", *ConstBuffer->Name);
				return;
			}

			MappedConstantBuffer* Buffer = MappedConstantBuffers.Get(ConstBuffer->Name);
			
			// Create a uniform buffer per swap chain image since each command buffer will reference it
			for (size_t BufferIndex = 0; BufferIndex < SwapChain->SwapChainImages.GetSize(); BufferIndex++)
			{
				VulkanBuffer* UniformBuffer = new VulkanBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer->HostBufferSize);

				Buffer->VulkanBuffers.Add(UniformBuffer);
			}
		}

	}

	void VulkanResourceSet::CreateBufferStorage()
	{
		// Create a mapped buffer for each constant buffer
		for(const ConstantBuffer* Buff : Info->ConstantBuffers)
		{
			MappedConstantBuffer* MappedBuffer = new MappedConstantBuffer;
			MappedBuffer->HostBufferSize = Ry::Std140Helpers::CalcBufferSize(Buff);
			MappedBuffer->HostDataBuffer = new uint8[MappedBuffer->HostBufferSize];

			Ry::Std140Helpers::MapData(MappedBuffer->UniformRefs, Buff);

			MappedConstantBuffers.Insert(Buff->Name, MappedBuffer);
		}
	}

	void VulkanResourceSet::CreateDescriptorSets(VulkanSwapChain* SwapChain)
	{
		const VulkanResourceSetDescription* VkResDesc = dynamic_cast<const VulkanResourceSetDescription*>(Info);
		assert(VkResDesc != nullptr);
		
		// Use the same layout for each descriptor set
		std::vector<VkDescriptorSetLayout> Layouts(SwapChain->SwapChainImages.GetSize(), VkResDesc->GetVkLayout());

		// Create a descriptor set for each frame in the swap chain
		VkDescriptorSetAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		AllocInfo.descriptorPool = SwapChain->DescriptorPool;
		AllocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChain->SwapChainImages.GetSize());
		AllocInfo.pSetLayouts = Layouts.data();

		DescriptorSets.SetSize(SwapChain->SwapChainImages.GetSize());
		if (vkAllocateDescriptorSets(GVulkanContext->GetLogicalDevice(), &AllocInfo, DescriptorSets.GetData()) != VK_SUCCESS)
		{
			Ry::Log->LogErrorf("Failed to allocate descriptor sets");
		}

		// Update each descriptor set image
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);
		for(int32 SwapChainImage = 0; SwapChainImage < VkSC->SwapChainImages.GetSize(); SwapChainImage++)
		{
			UpdateDescriptorSet(VkSC, SwapChainImage);
		}

	}

	void VulkanResourceSet::UpdateDescriptorSet(VulkanSwapChain* SwapChain, int32 SwapChainImageIndex)
	{
		Ry::ArrayList<VkWriteDescriptorSet> Descriptors;
		Ry::ArrayList<VkDescriptorBufferInfo> BufferInfos;
		Ry::ArrayList<VkDescriptorImageInfo> ImageInfos;

		for (const ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			MappedConstantBuffer* Mapped = MappedConstantBuffers.Get(ConstBuffer->Name);

			VkDescriptorBufferInfo BufferInfo{};
			BufferInfo.buffer = Mapped->VulkanBuffers[SwapChainImageIndex]->GetBufferObject();
			BufferInfo.offset = 0;
			BufferInfo.range = Mapped->HostBufferSize;
			BufferInfos.Add(BufferInfo);

			VkWriteDescriptorSet DescriptorWrite{};
			DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			DescriptorWrite.dstSet = DescriptorSets[SwapChainImageIndex];
			DescriptorWrite.dstBinding = C_BUFFER_OFFSET + ConstBuffer->Index;
			DescriptorWrite.dstArrayElement = 0;
			DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			DescriptorWrite.descriptorCount = 1;
			DescriptorWrite.pBufferInfo = BufferInfos.GetData() + (BufferInfos.GetSize() - 1);

			Descriptors.Add(DescriptorWrite);
		}

		for (int32 TextureBindingIndex = 0; TextureBindingIndex < Info->TextureBindings.GetSize(); TextureBindingIndex++)
		{
			const TextureBinding* Binding = Info->TextureBindings[TextureBindingIndex];

			if (!MappedTextures.Contains(Binding->Name))
			{
				continue;
			}

			MappedTexture* Mapping = MappedTextures.Get(Binding->Name);

			VkDescriptorImageInfo ImageInfo{};
			ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			ImageInfo.imageView = Mapping->Image;
			ImageInfo.sampler = Mapping->Sampler;

			ImageInfos.Add(ImageInfo);

			VkWriteDescriptorSet DescriptorWrite{};
			DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			DescriptorWrite.dstSet = DescriptorSets[SwapChainImageIndex];
			DescriptorWrite.dstBinding = T_BUFFER_OFFSET + Binding->Index;
			DescriptorWrite.dstArrayElement = 0;
			DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			DescriptorWrite.descriptorCount = 1;
			DescriptorWrite.pImageInfo = ImageInfos.GetData() + (ImageInfos.GetSize() - 1);

			Descriptors.Add(DescriptorWrite);
		}

		vkUpdateDescriptorSets(GVulkanContext->GetLogicalDevice(), Descriptors.GetSize(), Descriptors.GetData(), 0, nullptr);

	}

	void VulkanResourceSetDescription::SetShaderFlags(VkDescriptorSetLayoutBinding& Binding)
	{
		// Determine which flags to set for the passed in shader stages
		for (const ShaderStage& Stage : Stages)
		{
			if (Stage == ShaderStage::Vertex)
			{
				Binding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			}

			if (Stage == ShaderStage::Fragment)
			{
				Binding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}
		}
	}

	void VulkanResourceSetDescription::CreateDescription()
	{
		Ry::ArrayList<VkDescriptorSetLayoutBinding> LayoutBindings;

		for (int32 ConstantBufferIndex = 0; ConstantBufferIndex < ConstantBuffers.GetSize(); ConstantBufferIndex++)
		{
			const ConstantBuffer* ConstBuffer = ConstantBuffers[ConstantBufferIndex];

			VkDescriptorSetLayoutBinding ConstLayoutBinding;
			ConstLayoutBinding.binding = C_BUFFER_OFFSET + ConstBuffer->Index;
			ConstLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			ConstLayoutBinding.descriptorCount = 1; // todo: should we support this? this would allow the root descriptor to be an array
			ConstLayoutBinding.pImmutableSamplers = nullptr; // Optional
			ConstLayoutBinding.stageFlags = 0; // Calculate these down below

			SetShaderFlags(ConstLayoutBinding);

			LayoutBindings.Add(ConstLayoutBinding);
		}

		for (int32 TextureBindingIndex = 0; TextureBindingIndex < TextureBindings.GetSize(); TextureBindingIndex++)
		{
			const TextureBinding* Binding = TextureBindings[TextureBindingIndex];

			VkDescriptorSetLayoutBinding SamplerLayoutBinding{};
			SamplerLayoutBinding.binding = T_BUFFER_OFFSET + Binding->Index;
			SamplerLayoutBinding.descriptorCount = 1;
			SamplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			SamplerLayoutBinding.pImmutableSamplers = nullptr;
			SamplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			SetShaderFlags(SamplerLayoutBinding);

			LayoutBindings.Add(SamplerLayoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo LayoutInfo{};
		LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		LayoutInfo.bindingCount = LayoutBindings.GetSize();
		LayoutInfo.pBindings = LayoutBindings.GetData();

		if (vkCreateDescriptorSetLayout(GVulkanContext->GetLogicalDevice(), &LayoutInfo, nullptr, &SetLayout) != VK_SUCCESS)
		{
			Ry::Log->LogError("Failed to create descriptor set layout");
		}
	}

	void VulkanResourceSetDescription::DeleteDescription()
	{
		vkDestroyDescriptorSetLayout(GVulkanContext->GetLogicalDevice(), SetLayout, nullptr);
	}

	const VkDescriptorSetLayout& VulkanResourceSetDescription::GetVkLayout() const
	{
		return SetLayout;
	}
	
}


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

			if(!MappedTextures.contains(IdName))
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

		Ry::KeyIterator<Ry::String, MappedConstantBuffer*> Itr = MappedConstantBuffers.CreateKeyIterator();

		while(Itr)
		{
			(*Itr.Value())->Cleanup();

			delete (*Itr.Value());
			
			++Itr;
		}
		
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
			MappedConstantBuffer* Mapped = *MappedConstantBuffers.get(ConstantBuffer->Name);

			// Update the buffer data
			VulkanBuffer* UniformBuffForFrame = Mapped->VulkanBuffers[Frame];
			UniformBuffForFrame->UploadData(Mapped->HostDataBuffer, Mapped->HostBufferSize);
		}
	}

	void VulkanResourceSet::BindTexture(Ry::String TextureName, const Ry::Texture* Resource)
	{
		const VulkanTexture* VkTexture = dynamic_cast<const VulkanTexture*>(Resource);
		assert(VkTexture != nullptr);

		MappedTexture* Map = nullptr;

		if(MappedTextures.contains(TextureName))
		{
			Map = *MappedTextures.get(TextureName);
		}
		else
		{
			Map = new MappedTexture;
			MappedTextures.insert(TextureName, Map);
		}

		if(Map)
		{
			Map->Image = VkTexture->GetImageView();
			Map->Sampler = VkTexture->GetSampler();
		}

		// Update descriptor sets if we've already created them
		if(DescriptorSets.size() > 0)
		{
			VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);
			int FlightIndex = VkSC->GetCurrentImageIndex();

			if(FlightIndex < 0)
			{
				// Device wait, not in a frame so we have to update all (not efficient)			
				vkDeviceWaitIdle(GVulkanContext->GetLogicalDevice());

				for(int32 Index = 0; Index < VkSC->SwapChainImages.size(); Index++)
				{
					UpdateDescriptorSet(VkSC, Index);
				}
			}
			else
			{
				// Update this frame, mark others as dirty
				UpdateDescriptorSet(VkSC, FlightIndex);

				for (int32 Index = 0; Index < VkSC->SwapChainImages.size(); Index++)
				{
					if(Index != FlightIndex && !DirtyFrames.Contains(Index))
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
		if(!MappedConstantBuffers.contains(BufferName))
		{
			Ry::Log->LogErrorf("Constant buffer named %s does not exist", *BufferName);
			return;
		}
		
		MappedConstantBuffer* Buff = *MappedConstantBuffers.get(BufferName);

		if(!Buff->UniformRefs.contains(Id))
		{
			Ry::Log->LogErrorf("Uniform %s in constant buffer %s does not exist", *Id, *BufferName);
			return;
		}

		const BufferRef& Ref = *Buff->UniformRefs.get(Id);
		MemCpy(Buff->HostDataBuffer + Ref.Offset, Ref.SlotSize, Data, Ref.SlotSize);

		// Dirty the uniform frames
		VulkanSwapChain* VkSwap = dynamic_cast<VulkanSwapChain*>(Swap);
		for (int32 Frame = 0; Frame < VkSwap->SwapChainImages.size(); Frame++)
		{
			if (!UniformDirtyFrames.Contains(Frame))
			{
				UniformDirtyFrames.Add(Frame);
			}
		}
	}

	int32 SizeOf(const ShaderPrimitiveDataType& PrimData)
	{
		int32 FloatN = sizeof(float);

		switch (PrimData)
		{

			// 1
		case ShaderPrimitiveDataType::Float:
			return FloatN;

			// 2
		case ShaderPrimitiveDataType::Float2:
		case ShaderPrimitiveDataType::Float1x2:
		case ShaderPrimitiveDataType::Float2x1:
			return FloatN * 2;

			// 3
		case ShaderPrimitiveDataType::Float3:
		case ShaderPrimitiveDataType::Float1x3:
		case ShaderPrimitiveDataType::Float3x1:
			return FloatN * 3;

			// 4
		case ShaderPrimitiveDataType::Float4:
		case ShaderPrimitiveDataType::Float4x1:
		case ShaderPrimitiveDataType::Float1x4:
		case ShaderPrimitiveDataType::Float2x2:
			return FloatN * 4;

			// 6
		case ShaderPrimitiveDataType::Float3x2:
		case ShaderPrimitiveDataType::Float2x3:
			return FloatN * 6;

			// 8
		case ShaderPrimitiveDataType::Float4x2:
		case ShaderPrimitiveDataType::Float2x4:
			return FloatN * 8;

			// 9
		case ShaderPrimitiveDataType::Float3x3:
			return FloatN * 9;

			// 12
		case ShaderPrimitiveDataType::Float4x3:
		case ShaderPrimitiveDataType::Float3x4:
			return FloatN * 12;

			// 16
		case ShaderPrimitiveDataType::Float4x4:
			return FloatN * 16;

			// Unknown
		default:
			Ry::Log->LogError("Unknown element when calculating Vulkan size, uniforms probably won't work");
			return 0;
		}
	}

	int32 CalcBufferSize(const ConstantBuffer* Buffer)
	{
		int32 Offset = 0;

		for(const BufferMember* Member : Buffer->BufferMembers)
		{
			CalcMemberSize(Member, Offset);
		}

		return Offset;
	}

	void CalcMemberSize(const BufferMember* Member, int32& CurrentSize)
	{
		int32 Alignment = CalcAlignmentRequirement(Member);

		for (int32 Index = 0; Index < Member->ArraySize; Index++)
		{
			// Correct the alignment if needed
			if (CurrentSize % Alignment != 0)
			{
				CurrentSize = (static_cast<int32>(CurrentSize / Alignment) + 1) * Alignment;
			}

			// Check if this is a primitive or a struct
			const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Member);
			const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Member);

			if (Prim)
			{
				// todo: add support for arrays
				int32 DataSize = SizeOf(Prim->DataType) * Prim->ArraySize;
				CurrentSize += DataSize;
			}
			else if(Struct)
			{
				for(const BufferMember* Child : Struct->StructMembers)
				{
					CalcMemberSize(Child, CurrentSize);
				}				
			}
		}
	}

	int32 CalcAlignmentRequirement(const BufferMember* Root)
	{
		const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Root);
		const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Root);

		if (Prim)
		{
			return CalcAlignmentRequirement(Prim->DataType);
		}
		else if(Struct)
		{
			// Alignment is equal to the largest alignment among the children members

			int32 LargestAlignment = 0;
			
			for (const BufferMember* ChildMember : Struct->StructMembers)
			{
				int32 Alignment = CalcAlignmentRequirement(ChildMember);
				if (Alignment > LargestAlignment)
				{
					LargestAlignment = Alignment;
				}
			}

			return LargestAlignment;
		}
		
	}
		
	int32 CalcAlignmentRequirement(ShaderPrimitiveDataType Primitive)
	{
		// This will work independent of if the primitive data type is an array or not

		int32 FloatN = sizeof(float);
		
		// Taken from https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/html/vkspec.html#interfaces-resources
		switch (Primitive)
		{
	
		/* Arrays have the alignment of their primitive type */
		case ShaderPrimitiveDataType::Float:
			return FloatN;
	
		/* All 2 column matrix types get 2N alignment */
		case ShaderPrimitiveDataType::Float1x2:
		case ShaderPrimitiveDataType::Float2x2:
		case ShaderPrimitiveDataType::Float3x2:
		case ShaderPrimitiveDataType::Float4x2:
		case ShaderPrimitiveDataType::Float2:
			return 2 * FloatN;
			break;
	
		/* 3 and 4 column matrices get same alignment as 3/4 element vectors */
		case ShaderPrimitiveDataType::Float1x3:
		case ShaderPrimitiveDataType::Float2x3:
		case ShaderPrimitiveDataType::Float3x3:
		case ShaderPrimitiveDataType::Float4x3:
		case ShaderPrimitiveDataType::Float1x4:
		case ShaderPrimitiveDataType::Float2x4:
		case ShaderPrimitiveDataType::Float3x4:
		case ShaderPrimitiveDataType::Float4x4:
		case ShaderPrimitiveDataType::Float3:
		case ShaderPrimitiveDataType::Float4:
			return 4 * FloatN;
			
		default:
			Ry::Log->LogError("Unknown element when calculating Vulkan alignment, uniforms probably won't work");
			return 0;
		}
		
	}
	
	void VulkanResourceSet::CreateUniformBuffers(VulkanSwapChain* SwapChain)
	{
		for(const ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			if (!MappedConstantBuffers.contains(ConstBuffer->Name))
			{
				Ry::Log->LogErrorf("Mapped constant buffer not yet created for %s", *ConstBuffer->Name);
				return;
			}

			MappedConstantBuffer* Buffer = *MappedConstantBuffers.get(ConstBuffer->Name);
			
			// Create a uniform buffer per swap chain image since each command buffer will reference it
			for (size_t BufferIndex = 0; BufferIndex < SwapChain->SwapChainImages.size(); BufferIndex++)
			{
				VulkanBuffer* UniformBuffer = new VulkanBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer->HostBufferSize);

				Buffer->VulkanBuffers.Add(UniformBuffer);
			}
		}

	}

	void VulkanResourceSet::MapData(MappedConstantBuffer* Dst, const ConstantBuffer* Src)
	{
		Ry::String ParentID = "";
		int32 Marker = 0;

		for(const BufferMember* Member : Src->BufferMembers)
		{
			MapData_Helper(Dst, Member, ParentID, Marker);
		}	
	}

	void VulkanResourceSet::MapData_Helper(MappedConstantBuffer* Dst, const BufferMember* Member, Ry::String ParentId, int32& Marker)
	{
		int32 Alignment = CalcAlignmentRequirement(Member);

		for (int32 ChildIndex = 0; ChildIndex < Member->ArraySize; ChildIndex++)
		{
			// Do alignment correction here
			if (Marker % Alignment != 0)
			{
				Marker = (static_cast<int32>(Marker / Alignment) + 1) * Alignment;
			}

			Ry::String DataId;

			// Determine data ID based on array status
			if (Member->ArraySize == 1)
			{
				DataId = Member->Id;
			}
			else
			{
				DataId = Member->Id + "[" + Ry::to_string(ChildIndex) + "]";
			}

			Ry::String FullId = ParentId.IsEmpty() ? DataId : ParentId + "." + DataId;
			
			BufferRef URef;
			URef.Offset = Marker;

			const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Member);
			const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Member);

			if (Prim)
			{
				URef.bIsPrimitive = true;
				URef.SlotSize = SizeOf(Prim->DataType);

				// Now add the size of the primitive. Primitives are the only things that contribute to the actual size of the buffer, structs are just composite
				Marker += URef.SlotSize;
			}
			else if(Struct)
			{
				URef.bIsPrimitive = false;

				for (const BufferMember* ChildMember : Struct->StructMembers)
				{
					// Map the data for the child member
					MapData_Helper(Dst, ChildMember, FullId, Marker);
				}
			}

			Dst->UniformRefs.insert(FullId, URef);
		}

	}

	void VulkanResourceSet::CreateBufferStorage()
	{
		// Create a mapped buffer for each constant buffer
		for(const ConstantBuffer* Buff : Info->ConstantBuffers)
		{
			MappedConstantBuffer* MappedBuffer = new MappedConstantBuffer;
			MappedBuffer->HostBufferSize = CalcBufferSize(Buff);
			MappedBuffer->HostDataBuffer = new uint8[MappedBuffer->HostBufferSize];

			MapData(MappedBuffer, Buff);

			MappedConstantBuffers.insert(Buff->Name, MappedBuffer);
		}
		

		// Map IDs to offsets in storage

		// Create the buffer storage
		//int32 Marker = 0;
		//return CreateBufferStorage(Info.DataLayout, HostDataBuffer, Marker);	
	}

	// UniformStorage* VulkanUniformBuffer::CreateBufferStorage(const BufferMember* Member, uint8* Data, int32& Marker)
	// {
	// 	// Recursively create children before creating self
	// 	Ry::Map<Ry::String, UniformStorage*> Children;
	// 	for(const BufferMember* ChildMember : DataLayout.ChildMembers)
	// 	{
	// 		int32 Alignment = CalcAlignmentRequirement(*ChildMember);
	//
	// 		for (int32 ChildIndex = 0; ChildIndex < ChildMember->ArraySize; ChildIndex++)
	// 		{
	// 			// Do alignment correction here
	// 			if (Marker % Alignment != 0)
	// 			{
	// 				Marker = (static_cast<int32>(Marker / Alignment) + 1) * Alignment;
	// 			}
	//
	// 			UniformStorage* NewChild = nullptr;
	//
	// 			if (ChildMember->bIsPrimitive)
	// 			{
	// 				NewChild = new UniformStorage(true, Marker, Data);
	//
	// 				// Now add the size of the primitive. Primitives are the only things that contribute to the actual size of the buffer, structs are just composite
	// 				Marker += SizeOf(ChildMember->DataType);
	// 			}
	// 			else
	// 			{
	// 				NewChild = CreateBufferStorage(*ChildMember, Data, Marker);
	// 			}
	//
	// 			if(ChildMember->ArraySize == 1)
	// 			{
	// 				Children.insert(ChildMember->Id, NewChild);
	// 			}
	// 			else
	// 			{
	// 				Children.insert(ChildMember->Id + "[" + Ry::to_string(ChildIndex) + "]", NewChild);
	// 			}
	// 		}
	//
	// 	}
	//
	// 	return new UniformStorage(false, Marker, Data, Children);
	// }

	void VulkanResourceSet::PrintTabs(int32 Tabs)
	{
		for (int32 Tab = 0; Tab < Tabs; Tab++)
		{
			std::cout << "\t";
		}
	}
	
	void VulkanResourceSet::PrintData()
	{
		// Print data for all descriptor bindings

		// Start with const buffers
		for(const ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			for(const BufferMember* Member : ConstBuffer->BufferMembers)
			{
				int32 Offset = 0;
				PrintData(Member, 0, Offset);
			}
		}
	}
	
	void VulkanResourceSet::PrintData(const BufferMember* Member, int32 TabCount, int32& Offset)
	{
		const PrimitiveBufferMember* Prim = dynamic_cast<const PrimitiveBufferMember*>(Member);
		const StructBufferMember* Struct = dynamic_cast<const StructBufferMember*>(Member);

		PrintTabs(TabCount);
		if(Prim)
		{

			int32 StartOffset = Offset;
			int32 Alignment = CalcAlignmentRequirement(Member);
			// Do alignments for array indicies
			for (int Index = 0; Index < Member->ArraySize; Index++)
			{
				if (Offset % Alignment != 0)
				{
					Offset = (static_cast<int32>(Offset / Alignment) + 1) * Alignment;
				}
				Offset += SizeOf(Prim->DataType);
			}
			int32 EndOffset = Offset;

			std::cout << *ToString(Prim->DataType) << " " << *Prim->Id;

			if(Prim->ArraySize > 1)
			{
				std::cout << "[" << Prim->ArraySize << "]";
			}

			std::cout << "; offset=[" << StartOffset << ", " << EndOffset << "] alignment=" << Alignment << std::endl;
		}
		else if(Struct)
		{
			int32 StructSize = 0;
			CalcMemberSize(Struct, StructSize);

			int32 Alignment = CalcAlignmentRequirement(Member);
			if (Offset % Alignment != 0)
			{
				Offset = (static_cast<int32>(Offset / Alignment) + 1) * Alignment;
			}
			int32 StartOffset = Offset;
			int32 EndOffset = StartOffset + Member->ArraySize * StructSize;

			PrintTabs(TabCount);
			std::cout << "struct " << *Prim->Id;
			if (Prim->ArraySize > 1)
			{
				PrintTabs(TabCount);
				std::cout << "[" << Prim->ArraySize << "]";
			}

			std::cout << " offset = [" << StartOffset << ", " << EndOffset << "] alignment = " << Alignment << std::endl;

			PrintTabs(TabCount);
			std::cout << "\n{" << std::endl;
			for (const BufferMember* ChildMember : Struct->StructMembers)
			{
				PrintData(ChildMember, TabCount + 1, Offset);
			}
			PrintTabs(TabCount);
			std::cout << "\n}" << std::endl;

		}
		
	}

	void VulkanResourceSet::CreateDescriptorSets(VulkanSwapChain* SwapChain)
	{
		const VulkanResourceSetDescription* VkResDesc = dynamic_cast<const VulkanResourceSetDescription*>(Info);
		assert(VkResDesc != nullptr);
		
		// Use the same layout for each descriptor set
		std::vector<VkDescriptorSetLayout> Layouts(SwapChain->SwapChainImages.size(), VkResDesc->GetVkLayout());

		// Create a descriptor set for each frame in the swap chain
		VkDescriptorSetAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		AllocInfo.descriptorPool = SwapChain->DescriptorPool;
		AllocInfo.descriptorSetCount = static_cast<uint32_t>(SwapChain->SwapChainImages.size());
		AllocInfo.pSetLayouts = Layouts.data();

		DescriptorSets.resize(SwapChain->SwapChainImages.size());
		if (vkAllocateDescriptorSets(GVulkanContext->GetLogicalDevice(), &AllocInfo, DescriptorSets.data()) != VK_SUCCESS)
		{
			Ry::Log->LogErrorf("Failed to allocate descriptor sets");
		}

		// Update each descriptor set image
		VulkanSwapChain* VkSC = dynamic_cast<VulkanSwapChain*>(Swap);
		for(int32 SwapChainImage = 0; SwapChainImage < VkSC->SwapChainImages.size(); SwapChainImage++)
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
			MappedConstantBuffer* Mapped = *MappedConstantBuffers.get(ConstBuffer->Name);

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

			if (!MappedTextures.contains(Binding->Name))
			{
				continue;
			}

			MappedTexture* Mapping = *MappedTextures.get(Binding->Name);

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


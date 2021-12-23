#include "GLResources.h"

#include "GLFrameBuffer.h"
#include "Core/Globals.h"
#include "GLTexture.h"

namespace Ry
{


	GLResources::GLResources(const ResourceLayout* CreateInfo, SwapChain* SC):
	ResourceSet(CreateInfo, SC)
	{
		for (ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			// Create new mapped uniform buffer
			MappedUniformBuffer* NewBuff = new MappedUniformBuffer;
			NewBuff->ConstBuffer = ConstBuffer;
			NewBuff->HostBufferSize = Ry::Std140Helpers::CalcBufferSize(ConstBuffer);
			NewBuff->HostDataBuffer = new uint8[NewBuff->HostBufferSize];

			glGenBuffers(1, &NewBuff->Ubo);

			// Give initial buffer data
			glBindBuffer(GL_UNIFORM_BUFFER, NewBuff->Ubo);
			glBufferData(GL_UNIFORM_BUFFER, NewBuff->HostBufferSize, nullptr, GL_DYNAMIC_DRAW); // todo: change this?
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			Ry::Std140Helpers::MapData(NewBuff->UniformRefs, ConstBuffer);
			MappedUniformBuffers.insert(ConstBuffer->Name, NewBuff);
		}
	}

	void GLResources::CreateBuffer()
	{

	}

	void GLResources::DeleteBuffer()
	{
		for (ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			MappedUniformBuffer* Buff = *MappedUniformBuffers.get(ConstBuffer->Name);

			glDeleteBuffers(1, &Buff->Ubo);

			delete[] Buff->HostDataBuffer;

			delete Buff;
		}
	}

	bool GLResources::Update()
	{
		return false;
	}

	void GLResources::FlushBuffer(int32 Frame)
	{
		// Send data
		
		for(const ConstantBuffer* ConstBuffer : Info->ConstantBuffers)
		{
			MappedUniformBuffer* Buff = *MappedUniformBuffers.get(ConstBuffer->Name);

			glBindBuffer(GL_UNIFORM_BUFFER, Buff->Ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, Buff->HostBufferSize, Buff->HostDataBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glBindBufferBase(GL_UNIFORM_BUFFER, ConstBuffer->Index, Buff->Ubo);
		}
	}

	void GLResources::BindFrameBufferAttachment(Ry::String TextureName, const Ry::ColorAttachment* ColorAttachment)
	{
		if (const GLColorAttachment* GLAttachment= dynamic_cast<const GLColorAttachment*>(ColorAttachment))
		{
			GLuint AttachmentHandle = GLAttachment->AttachmentHandle;

			// TODO: Can a framebuffer color attachment be anything other than a GL_TEXTURE_2D?
			BindTexture(TextureName, AttachmentHandle, GL_TEXTURE_2D);
		}
		else
		{
			Ry::Log->LogError("GLResources::BindFrameBufferAttachment: Invalid GL color attachment");
		}

	}

	void GLResources::BindTexture(Ry::String TextureName, const Ry::Texture* Resource)
	{
		if(const GLTexture* GLTex = dynamic_cast<const GLTexture*>(Resource))
		{
			BindTexture(TextureName, GLTex->GetHandle(), GLTex->GetTarget());
		}
		else
		{
			Ry::Log->LogError("GLResources::BindTexture: Passed in non-Gl texture!");
		}
	}

	void GLResources::BindTexture(Ry::String TextureName, GLuint Handle, GLuint Target)
	{
		MappedTexture* NewMappedTexture = nullptr;

		if (MappedTextures.contains(TextureName))
		{
			NewMappedTexture = *MappedTextures.get(TextureName);
		}
		else
		{
			NewMappedTexture = new MappedTexture;
			MappedTextures.insert(TextureName, NewMappedTexture);
			NewMappedTexture->Slot = Textures;

			Textures++;
		}

		if (NewMappedTexture)
		{
			NewMappedTexture->Texture = Handle;
			NewMappedTexture->Target = Target;
		}
	}

	void GLResources::SetConstant(Ry::String BufferName, Ry::String Id, const void* Data)
	{
		if (!MappedUniformBuffers.contains(BufferName))
		{
			Ry::Log->LogErrorf("Uniform buffer named %s does not exist", *BufferName);
			return;
		}

		MappedUniformBuffer* Buff = *MappedUniformBuffers.get(BufferName);

		if (!Buff->UniformRefs.Contains(Id))
		{
			Ry::Log->LogErrorf("Uniform %s in constant buffer %s does not exist", *Id, *BufferName);
			return;
		}

		const BufferRef& Ref = Buff->UniformRefs.Get(Id);
		MemCpy(Buff->HostDataBuffer + Ref.Offset, Ref.SlotSize, Data, Ref.SlotSize);
	}


}

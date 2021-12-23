#pragma once

#include "RenderingResource.h"
#include "glew.h"
#include "Data/Map.h"
#include "FrameBuffer.h"
#include "OpenGLGen.h"

namespace Ry
{

	class OPENGL_MODULE GLResourceDescription : public ResourceLayout
	{
	public:

		GLResourceDescription(const Ry::ArrayList<ShaderStage>& Stages, int32 SetIndex = 0) : ResourceLayout(Stages, SetIndex){}
		virtual ~GLResourceDescription() = default;
		
		void CreateDescription() override {}
		void DeleteDescription() override {}
	};

	struct MappedUniformBuffer
	{
		ConstantBuffer* ConstBuffer = nullptr;
		Ry::OAHashMap<Ry::String, BufferRef> UniformRefs;
		uint8* HostDataBuffer;
		uint32 HostBufferSize;
		GLuint Ubo;
	};

	struct MappedTexture
	{
		int32 Slot;
		GLuint Texture;
		GLuint Target;
	};

	class OPENGL_MODULE GLResources : public ResourceSet
	{
	public:

		GLResources(const ResourceLayout* CreateInfo, SwapChain* SC);

		void CreateBuffer() override;
		void DeleteBuffer() override;
		bool Update() override;
		void FlushBuffer(int32 Frame) override;

		void BindFrameBufferAttachment(Ry::String TextureName, const Ry::ColorAttachment* ColorAttachment) override;
		void BindTexture(Ry::String TextureName, const Ry::Texture* Resource) override;

		void SetConstant(Ry::String BufferName, Ry::String Id, const void* Data) override;

		MappedTexture* GetMappedTexture(const Ry::String& Name)
		{
			return *MappedTextures.get(Name);
		}

	private:

		void BindTexture(Ry::String TextureName, GLuint Handle, GLuint Target);

		Map<Ry::String, MappedUniformBuffer*> MappedUniformBuffers;

		int32 Textures = 0;
		Ry::Map<Ry::String, MappedTexture*> MappedTextures;

	};
	
}

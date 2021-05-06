#pragma once

#include "Core/Core.h"
#include "Core/Globals.h"
#include "Color.h"
#include "Interface/Shader.h"

namespace Ry
{

	struct ColorSource
	{
		// Ry::FrameBuffer* Buffer = nullptr;
		int32 ColorAttachment = 0;
		Ry::String UniformName = "ColorSource0";
	};

	struct DepthSource
	{
		// Ry::FrameBuffer* Buffer = nullptr;
		Ry::String UniformName = "DepthSource";
	};

	struct StencilSource
	{
		// Ry::FrameBuffer* Buffer = nullptr;
		Ry::String UniformName = "StencilSource";
	};

	class RENDERING_MODULE RenderingPass
	{
	public:

		RenderingPass()
		{
			this->Enabled = false;
			this->DepthTestEnabled = true;
			// this->BlendingParameters.BlendingEnabled = false;
		}

		virtual ~RenderingPass()
		{
			
		}

		virtual void BeginPass()
		{	
			// Ry::RenderAPI->SetDepthTestEnabled(DepthTestEnabled);
			//
			// if(BlendingParameters.BlendingEnabled)
			// {
			// 	Ry::RenderAPI->SetBlendState(BlendingParameters);
			// }
			//
			// int32 ViewportWidth = Ry::GetViewportWidth();
			// int32 ViewportHeight = Ry::GetViewportHeight();
			//
			// Ry::RenderAPI->SetViewport(0, 0, ViewportWidth, ViewportHeight);
			//
			// Enabled = true;
		}
		
		virtual void EndPass()
		{	
			Enabled = false;
		}

		bool IsEnabled()
		{
			return Enabled;
		}

		bool IsDepthTestEnabled()
		{
			return DepthTestEnabled;
		}

		// BlendParameters& GetBlendParameters()
		// {
		// 	return BlendingParameters;
		// }

		void SetDepthTestEnabled(bool DepthTestEnabled)
		{
			this->DepthTestEnabled = DepthTestEnabled;
		}

		void SetBlendingEnabled(bool BlendingEnabled)
		{
			// this->BlendingParameters.BlendingEnabled = BlendingEnabled;
		}

		// void SetBlendMode(BlendMode Mode)
		// {
		// 	this->BlendingParameters.Mode = Mode;
		// }
		//
		// void SetSourceColorBlendFactor(BlendFactor SourceFactor)
		// {
		// 	this->BlendingParameters.SourceColorFactor = SourceFactor;
		// }
		//
		// void SetDestinationColorBlendFactor(BlendFactor DestFactor)
		// {
		// 	this->BlendingParameters.DestColorFactor = DestFactor;
		// }
		//
		// void SetSourceAlphaBlendFactor(BlendFactor SourceFactor)
		// {
		// 	this->BlendingParameters.SourceAlphaFactor = SourceFactor;
		// }
		//
		// void SetDestinationAlphaBlendFactor(BlendFactor DestFactor)
		// {
		// 	this->BlendingParameters.DestAlphaFactor = DestFactor;
		// }
		//
		// void SetBlendParameters(const BlendParameters& BlendingParameters)
		// {
		// 	this->BlendingParameters = BlendingParameters;
		// }

	protected:
		
		bool Enabled;
		bool DepthTestEnabled;
		// BlendParameters BlendingParameters;
	};

	class RENDERING_MODULE ImposePass : public RenderingPass
	{
	public:

		ImposePass():
		RenderingPass(),
		ImposeShader(nullptr)
		{
			// ScreenSpaceMesh = Ry::BuildScreenSpaceMesh1P1UV(nullptr);
		}
		
		virtual ~ImposePass()
		{
			// delete ScreenSpaceMesh;
		}

		void SetImposeShader(Shader* ImposeShader)
		{
			//ScreenSpaceMesh->GetMeshData()->SetShaderAll(ImposeShader);
			this->ImposeShader = ImposeShader;
		}

		// void SetColorSource(FrameBuffer* FrameBuffer, int32 ColorAttachment)
		// {
		// 	// Unbind previous callback
		// 	if (Source.Buffer)
		// 	{
		// 	//	Source.Buffer->OnBufferResized.RemoveMemberFunction(this, &ImposePass::OnSourceBufferResize);
		// 	}
		//
		// 	// Setup the source buffer
		// 	this->Source.Buffer = FrameBuffer;
		// 	//this->Source.Buffer->OnBufferResized.AddMemberFunction(this, &ImposePass::OnSourceBufferResize);
		// 	this->Source.ColorAttachment = ColorAttachment;
		// }

		void Impose()
		{
			RenderingPass::BeginPass();

			int32 ViewportWidth = Ry::GetViewportWidth();
			int32 ViewportHeight = Ry::GetViewportHeight();

			// todo: uniforms aren't set using the shader anymore
			//ImposeShader->uniform_float("ScreenWidth", (float) ViewportWidth);
			//ImposeShader->uniform_float("ScreenHeight", (float) ViewportHeight);
			//ImposeShader->uniform_int32("USamples", Source.Buffer->GetSamples());
			// Source.Buffer->GetColorAttachment(Source.ColorAttachment)->Bind();

			// todo: mesh doesn't render directly anymore
			//ScreenSpaceMesh->Render(Ry::Primitive::TRIANGLE);
			
			RenderingPass::EndPass();
		}
		
	private:
		ColorSource Source;
		// Ry::Mesh* ScreenSpaceMesh;
		Ry::Shader* ImposeShader;
	};

	class RENDERING_MODULE OffScreenRenderingPass : public RenderingPass
	{
	public:

		OffScreenRenderingPass(int32 Samples):
		RenderingPass()
		{
			this->AutomaticallyBlit = false;

			// Enable a multisample color attachment for the frame buffer
			// Output = Ry::RenderAPI->make_framebuffer(Ry::GetViewportWidth(), Ry::GetViewportHeight(), Samples);
			// Output->CreateColorAttachment(0);

			SetClearColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
		}

		virtual ~OffScreenRenderingPass()
		{
			// delete Output;
		}

		void SetClearColor(const Ry::Color& ClearColor)
		{
			this->ClearColor = ClearColor;
		}

		// void ResizeBuffer(int32 Width, int32 Height)
		// {
		// 	Output->ResizeBuffer(Width, Height);
		// }
		//
		// void BeginPass() override
		// {
		// 	// Bind this frame buffer for off screen rendering
		//
		// 	Output->Bind();
		// 	Output->ClearAll(ClearColor.Red, ClearColor.Green, ClearColor.Blue, ClearColor.Alpha);
		//
		// 	RenderingPass::BeginPass();
		//
		// 	Ry::RenderAPI->SetMSAAEnabled(Output->GetSamples() > 1);
		//
		// 	Ry::RenderAPI->SetViewport(0, 0, Output->GetIntendedWidth(), Output->GetIntendedHeight());
		// }
		//
		// void EndPass() override
		// {
		// 	RenderingPass::EndPass();
		//
		// 	if (AutomaticallyBlit)
		// 	{
		// 		Output->BlitFramebuffer(nullptr, true, false, false, 0, 0);
		// 	}
		//
		// 	// Go back to the default framebuffer
		// 	Ry::RenderAPI->BindDefaultFramebuffer();
		// }

		// FrameBuffer* GetOutputBuffer()
		// {
		// 	return Output;
		// }
		
	private:
		// FrameBuffer* Output;
		Ry::Color ClearColor;
		bool AutomaticallyBlit;
	};

	class RENDERING_MODULE PostProcessPass : public OffScreenRenderingPass
	{
	public:

		PostProcessPass():
		OffScreenRenderingPass(0),
		// ScreenSpaceMesh(nullptr),
		PostProcessShader(nullptr)
		{
			// ScreenSpaceMesh = Ry::BuildScreenSpaceMesh1P1UV(nullptr);
		}

		void SetPostProcessShader(Shader* PostProcess)
		{
			//ScreenSpaceMesh->GetMeshData()->SetShaderAll(PostProcess);
			this->PostProcessShader = PostProcess;
		}

		// void AddColorSource(Ry::FrameBuffer* SourceBuffer, int32 ColorAttachment, const Ry::String& UniformName)
		// {
		// 	ColorSource Source;
		// 	Source.Buffer = SourceBuffer;
		// 	Source.ColorAttachment = ColorAttachment;
		// 	Source.UniformName = UniformName;
		//
		// 	ColorSources.Add(Source);
		// }
		//
		// void AddDepthSource(Ry::FrameBuffer* SourceBuffer, int32 ColorAttachment, const Ry::String& UniformName)
		// {
		// 	DepthSource Source;
		// 	Source.Buffer = SourceBuffer;
		// 	Source.UniformName = UniformName;
		//
		// 	DepthSources.Add(Source);
		// }
		//
		// void AddStencilSource(Ry::FrameBuffer* SourceBuffer, int32 ColorAttachment, const Ry::String& UniformName)
		// {
		// 	StencilSource Source;
		// 	Source.Buffer = SourceBuffer;
		// 	Source.UniformName = UniformName;
		//
		// 	StencilSources.Add(Source);
		// }

		void Process()
		{
			BeginPass();

			int32 TextureUnit = 0;

			for(const ColorSource& ColorSource : ColorSources)
			{
				// ColorSource.Buffer->GetColorAttachment(ColorSource.ColorAttachment)->Bind(TextureUnit);
				TextureUnit++;
			}

			for (const DepthSource& DepthSource : DepthSources)
			{
				// DepthSource.Buffer->GetDepthAttachment()->Bind(TextureUnit);
				TextureUnit++;
			}

			for (const StencilSource& StencilSource : StencilSources)
			{
				// StencilSource.Buffer->GetStencilAttachment()->Bind(TextureUnit);
				TextureUnit++;
			}

			// Render post process effect
			// todo: mesh doesn't render directly anymore
			//ScreenSpaceMesh->Render(Primitive::TRIANGLE);

			EndPass();
		}

	private:
		// Ry::Mesh* ScreenSpaceMesh;
		Ry::Shader* PostProcessShader;

		// Texture sources
		Ry::ArrayList<ColorSource> ColorSources;
		Ry::ArrayList<DepthSource> DepthSources;
		Ry::ArrayList<StencilSource> StencilSources;
	};
	
}

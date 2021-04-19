#include "RenderingEngine.h"
#include "2D/Batch/Batch.h"
#include "RenderingPass.h"
#include "RenderPipeline.h"
#include "Asset.h"
#include "Interface2/RenderAPI.h"

//#include "Core/Application/Application.h"

namespace Ry
{
	
	// Rendering passes
	OffScreenRenderingPass* UIPass = nullptr;
	ImposePass* UIImposePass = nullptr;
	OffScreenRenderingPass* ScenePass = nullptr;
	ImposePass* SceneImposePass = nullptr;
	RenderingPass* TextPass = nullptr;
	UniquePtr<RenderPipeline> ObjectPipeline;

	// Shaders
	Ry::Map<String, Ry::Shader2*> CompiledShaders;

	// Batchers
	Batch* TextBatcher = nullptr;
	Batch* ShapeBatcher = nullptr;
	//TextureBatch* TextureBatcher = nullptr;
	//ShapeBatch* ShapeBatcher = nullptr;
	
	void InitRenderingEngine()
	{		
		// Compile standard shaders
		Ry::Log->Log("Compiling shaders");
		{
			CompileShader("Impose", "/Engine/Shaders/Vertex/Impose.glv", "/Engine/Shaders/Fragment/Impose.glf");
			CompileShader("PhongDirectional", "/Engine/Shaders/Vertex/Phong.glv", "/Engine/Shaders/Fragment/PhongDirectional.glf");
			CompileShader("Texture2D", "/Engine/Shaders/Vertex/base.glv", "/Engine/Shaders/Fragment/texture_tint.glf");

			CompileShader("Shape", "Vertex/Shape", "Fragment/Shape");
			CompileShader("Font", "Vertex/Font", "Fragment/Font");

			
		}

		// Initialize batches
		{
			//ShapeBatcher = new Batch(Ry::VF1P1C);
			//ShapeBatcher->SetShader("Shape");

			//TextBatcher = new Batch(Ry::VF1P1UV1C);
			//TextBatcher->SetShader("Font");

			//TextureBatcher = new TextureBatch;
			//ShapeBatcher = new ShapeBatch;
		}

		// Initialize passes
/*		{
			ScenePass = new Ry::OffScreenRenderingPass(4);
			ScenePass->SetDepthTestEnabled(true);
			ScenePass->SetBlendingEnabled(true);
			ScenePass->SetClearColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
			ScenePass->GetOutputBuffer()->CreateDepthAttachment();

			// User interface rendering pass. We're rendering the UI offscreen for hardware multisample support.
			UIPass = new Ry::OffScreenRenderingPass(8);
			UIPass->SetDepthTestEnabled(false);
			UIPass->SetBlendingEnabled(true);
			UIPass->SetBlendMode(BlendMode::ADDITIVE);
			UIPass->SetSourceColorBlendFactor(BlendFactor::SRC_ALPHA);
			UIPass->SetDestinationColorBlendFactor(BlendFactor::ONE_MINUS_SRC_ALPHA);
			UIPass->SetSourceAlphaBlendFactor(BlendFactor::SRC_ALPHA);
			UIPass->SetDestinationAlphaBlendFactor(BlendFactor::ONE_MINUS_SRC_ALPHA);
			UIPass->SetClearColor(Color(0.0f, 0.0f, 0.0f, 0.0f));

			TextPass = new Ry::RenderingPass();
			TextPass->SetDepthTestEnabled(false);
			TextPass->SetBlendingEnabled(true);
			TextPass->SetBlendMode(BlendMode::ADDITIVE);
			TextPass->SetSourceColorBlendFactor(BlendFactor::SRC_ALPHA);
			TextPass->SetDestinationColorBlendFactor(BlendFactor::ONE_MINUS_SRC_ALPHA);
			TextPass->SetSourceAlphaBlendFactor(BlendFactor::ZERO);
			TextPass->SetDestinationAlphaBlendFactor(BlendFactor::ONE);

			// Use interface scene overlay rendering pass
			UIImposePass = new Ry::ImposePass;
			UIImposePass->SetDepthTestEnabled(false);
			UIImposePass->SetBlendingEnabled(true);
			UIImposePass->SetBlendMode(BlendMode::ADDITIVE);
			UIImposePass->SetSourceColorBlendFactor(BlendFactor::SRC_ALPHA);
			UIImposePass->SetDestinationColorBlendFactor(BlendFactor::ONE_MINUS_SRC_ALPHA);
			UIImposePass->SetImposeShader(GetShader("Impose"));
			UIImposePass->SetColorSource(UIPass->GetOutputBuffer(), 0);

			// Use interface scene overlay rendering pass
			SceneImposePass = new Ry::ImposePass;
			SceneImposePass->SetDepthTestEnabled(false);
			SceneImposePass->SetBlendingEnabled(true);
			SceneImposePass->SetBlendMode(BlendMode::ADDITIVE);
			SceneImposePass->SetSourceColorBlendFactor(BlendFactor::SRC_ALPHA);
			SceneImposePass->SetDestinationColorBlendFactor(BlendFactor::ONE_MINUS_SRC_ALPHA);
			SceneImposePass->SetImposeShader(GetShader("Impose"));
			SceneImposePass->SetColorSource(ScenePass->GetOutputBuffer(), 0);
		}

		// Initialize rendering pipeline to a forward renderer
		ObjectPipeline = MakeUnique<ForwardRenderer>();
		*/
		// Hook into resize events
		Ry::OnWindowResize.AddFunction(&HandleResize);
	}

	void QuitRenderingEngine()
	{
		delete ShapeBatcher;
		delete TextBatcher;
		//delete ShapeBatcher;
		//delete TextureBatcher;
	}

	void HandleResize(int32 Width, int32 Height)
	{
		// Resize frame buffers
		UIPass->ResizeBuffer(Width, Height);
		ScenePass->ResizeBuffer(Width, Height);

		ShapeBatcher->Resize(Width, Height);
		TextBatcher->Resize(Width, Height);

		// Resize standard batches
	//	ShapeBatcher->ResizeProjection(Width, Height);
	//	TextureBatcher->ResizeProjection(Width, Height);
	}

	Shader2* CompileShader(const String& Name, Ry::String VertexLoc, Ry::String FragmentLoc)
	{
		Ry::Shader2* Result = Ry::NewRenderAPI->CreateShader(VertexLoc, FragmentLoc);
//		Shader2* Result = Ry::RenderAPI->make_shader(Format, );
		CompiledShaders.insert(Name, Result);

		return Result;
	}

	Shader2* GetShader(const String& Name)
	{
		if(CompiledShaders.contains(Name))
		{
			return *CompiledShaders.get(Name);
		}
		else
		{
			return nullptr;
		}
	}
	
}
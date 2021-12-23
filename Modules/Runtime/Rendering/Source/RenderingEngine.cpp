#include "RenderingEngine.h"
#include "2D/Batch/Batch.h"
#include "Asset.h"
#include "Interface/RenderAPI.h"
#include "Bitmap.h"
#include "Interface/Texture.h"

namespace Ry
{

	Ry::Texture* DefaultTexture = nullptr;

	// Shaders
	Ry::Map<String, Ry::Shader*> CompiledShaders;

	void InitRenderingEngine()
	{		
		// Compile standard shaders
		Ry::Log->Log("Compiling shaders");
		{
			CompileShader("Texture", "Vertex/Texture", "Fragment/Texture");
			CompileShader("Shape", "Vertex/Shape", "Fragment/Shape");
			CompileShader("Font", "Vertex/Font", "Fragment/Font");
		}

		// Create 1x1 default texture
		Ry::Bitmap DefaultTextureBmp (1, 1, PixelStorage::FOUR_BYTE_RGBA);
		DefaultTextureBmp.SetPixel(0, 0, 0xFFFFFFFF);
		DefaultTexture = Ry::RendAPI->CreateTexture(TextureFiltering::Nearest);
		DefaultTexture->Data(&DefaultTextureBmp);
	}

	void QuitRenderingEngine()
	{
		DefaultTexture->DeleteTexture();
		delete DefaultTexture;
	}

	Shader* GetOrCompileShader(const String& Name, Ry::String Vertex, Ry::String Fragment)
	{
		Shader* Cur = GetShader(Name);

		if(!Cur)
		{
			Cur = CompileShader(Name, Vertex, Fragment);
		}

		return Cur;
	}

	Shader* CompileShader(const String& Name, Ry::String VertexLoc, Ry::String FragmentLoc)
	{
		Ry::Shader* Result = Ry::RendAPI->CreateShader(VertexLoc, FragmentLoc);
		CompiledShaders.insert(Name, Result);

		return Result;
	}

	Shader* GetShader(const String& Name)
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
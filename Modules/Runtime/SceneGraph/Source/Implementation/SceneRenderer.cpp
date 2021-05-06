#include "SceneRenderer.h"
#include "Interface/RenderCommand.h"
#include "SceneGraph.h"
#include "Interface/RenderingResource.h"
#include "ScenePrimitive.h"
#include "Interface/RenderAPI.h"
#include "Application.h"
#include "Mesh.h"
#include "Material.h"
#include "RenderingEngine.h"

namespace Ry
{

	SceneRenderer::SceneRenderer(Ry::SceneGraph* TarScene)
	{
		this->TargetScene = TarScene;
		this->bDirty = false;

		// Setup scene callbacks
		TargetScene->OnPrimitiveAdded.AddMemberFunction(this, &SceneRenderer::OnPrimitiveAdded);
		TargetScene->OnPrimitiveRemoved.AddMemberFunction(this, &SceneRenderer::OnPrimitiveRemoved);

		this->PrimResDesc = nullptr;
		this->SceneResDesc = nullptr;
		this->MaterialDesc = nullptr;
		this->SceneResources = nullptr;
	}

	SceneRenderer::~SceneRenderer()
	{
		// Remove scene callbacks (if scene is valid) we don't want the scene to call our invalid callbacks

		if(TargetScene)
		{
			TargetScene->OnPrimitiveAdded.RemoveMemberFunction(this, &SceneRenderer::OnPrimitiveAdded);
			TargetScene->OnPrimitiveRemoved.RemoveMemberFunction(this, &SceneRenderer::OnPrimitiveRemoved);
		}
	}

	void SceneRenderer::Init()
	{
		// Create
		CreateResources();
	}

	PrimitiveResources* SceneRenderer::CreatePrimitiveResources(ScenePrimitive* Prim)
	{
		StaticMeshPrimitive* SMPrimitive = dynamic_cast<StaticMeshPrimitive*>(Prim);

		if (SMPrimitive)
		{
			// Allocate resources for primitive
			Ry::ResourceSet* PrimitiveRes = Ry::RendAPI->CreateResourceSet(PrimResDesc, Ry::app->GetSwapChain());
			{
				PrimitiveRes->SetMatConstant("Model", "Transform", Ry::id4());
			}
			PrimitiveRes->CreateBuffer();
			
			// Allocate resource for this primitive (renderer will have to set values)
			PrimitiveResources* Resources = new PrimitiveResources;
			Resources->SetPrimitiveSpecificResources(PrimitiveRes);
			Resources->SetVertexArray(SMPrimitive->GetMesh()->GetVertexArray());

			// Allocate resources for each mesh section
			MeshData* Data = SMPrimitive->GetMesh()->GetMeshData();
			for (int32 MeshSection = 0; MeshSection < Data->SectionCount; MeshSection++)
			{
				Ry::MeshSection& Sec = *Data->Sections.get(MeshSection);

				MaterialResources* SectionResources = new MaterialResources;
				SectionResources->StartIndex = Sec.StartIndex;
				SectionResources->IndexCount = Sec.Count;

				// Bind resources for material
				Ry::ResourceSet* MatRes = Ry::RendAPI->CreateResourceSet(MaterialDesc, Ry::app->GetSwapChain());
				SectionResources->MaterialSpecificResource = MatRes;
				{
					if (Sec.Mat->DiffuseTexture)
					{
						MatRes->SetFloatConstant("Material", "UseDiffuseTexture", 1.0f);
						MatRes->BindTexture("DiffuseTexture", Sec.Mat->DiffuseTexture);
					}
					else
					{
						// Don't use diffuse, bind default
						MatRes->SetFloatConstant("Material", "UseDiffuseTexture", 0.0f);
						MatRes->BindTexture("DiffuseTexture", Ry::DefaultTexture);
					}

					MatRes->SetMatConstant("Material", "DiffuseColor", Sec.Mat->Diffuse);
					MatRes->SetMatConstant("Material", "AmbientColor", Sec.Mat->Ambient);
					MatRes->SetMatConstant("Material", "SpecularColor", Sec.Mat->Specular);
				}
				MatRes->CreateBuffer();

				// Add all resource sets to the material
				SectionResources->Resources.Add(SceneResources);
				SectionResources->Resources.Add(PrimitiveRes);
				SectionResources->Resources.Add(LightResources);
				SectionResources->Resources.Add(MatRes);

				// Add resources to overall primitive
				Resources->AddMaterialResource(SectionResources);
			}

			return Resources;
		}
		else
		{
			Ry::Log->LogError("Primitive type not supported");
		}

		return nullptr;
	}

	Pipeline* SceneRenderer::SelectPipelineForPrimitive()
	{
		return nullptr;
	}

	void SceneRenderer::OnPrimitiveAdded(ScenePrimitive* Prim)
	{
		bDirty = true;

		// Create primitive resources
		PrimitiveResources* Res = CreatePrimitiveResources(Prim);

		if(!Res)
		{
			Ry::Log->LogErrorf("Failed to create primitive resources");
		}
		else
		{
			PrimResources.insert(Prim, Res);
		}
	}

	void SceneRenderer::OnPrimitiveRemoved(ScenePrimitive* Prim)
	{
		bDirty = true;

		// Destroy the primitive data
		if(PrimResources.contains(Prim))
		{
			PrimitiveResources* Res = *PrimResources.get(Prim);
			delete Res;

			PrimResources.remove(Prim);
		}
	}

	MaterialResources::MaterialResources()
	{
		this->StartIndex = 0;
		this->IndexCount = 0;
		this->MaterialSpecificResource = nullptr;
	}

	PrimitiveResources::PrimitiveResources()
	{
		this->VertArray = nullptr;
		this->PrimSpecificResource = nullptr;
	}

	PrimitiveResources::~PrimitiveResources()
	{
		// Destroy material specific resources
		for(MaterialResources* Res : MaterialRes)
		{
			Res->MaterialSpecificResource->DeleteBuffer();
			delete Res;
		}
	}

	const Ry::ArrayList<MaterialResources*>& PrimitiveResources::GetMaterialResources() const
	{
		return MaterialRes;
	}

	void PrimitiveResources::AddMaterialResource(Ry::MaterialResources* Res)
	{
		this->MaterialRes.Add(Res);
	}

	void PrimitiveResources::SetPrimitiveSpecificResources(ResourceSet* PrimSpec)
	{
		this->PrimSpecificResource = PrimSpec;
	}

	ResourceSet* PrimitiveResources::GetPrimitiveSpecificResources()
	{
		return PrimSpecificResource;
	}

	void PrimitiveResources::SetVertexArray(VertexArray* Verts)
	{
		this->VertArray = Verts;
	}

	VertexArray* PrimitiveResources::GetVertexArray()
	{
		return VertArray;
	}

	void PrimitiveResources::RecordDraw(Ry::CommandBuffer* DstBuffer)
	{
		// Draw each level piece
		int32 MaterialResourceCount = MaterialRes.GetSize();
		for (int32 MatResIndex = 0; MatResIndex < MaterialResourceCount; MatResIndex++)
		{
			Ry::MaterialResources* Resource = MaterialRes[MatResIndex];

			// Bind resources for this material
			DstBuffer->BindResources(Resource->Resources.GetData(), Resource->Resources.GetSize());

			// Draw the level piece
			// Ry::MeshSection& Sec = *StaticGeom->GetMeshData()->Sections.get(Piece.SectionIndex);
			// StaticCmdBuffer->DrawVertexArrayIndexed(StaticGeom->GetVertexArray(), Sec.StartIndex, Sec.Count);

			DstBuffer->DrawVertexArrayIndexed(VertArray, Resource->StartIndex, Resource->IndexCount);	
		}
		
	}
	
}

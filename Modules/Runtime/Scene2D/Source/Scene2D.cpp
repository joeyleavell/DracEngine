#include "Scene2D.h"
#include "Interface/RenderAPI.h"
#include "SwapChain.h"
#include "Interface/RenderCommand.h"

namespace Ry
{
	ScenePrimitive::ScenePrimitive(PrimitiveMobility Mobility)
	{
		ItemSet = MakeItemSet();
		PipelineId = "Texture";
		this->Mobility = Mobility;
	}

	Ry::SharedPtr<BatchItemSet> ScenePrimitive::GetItemSet()
	{
		return ItemSet;
	}

	PrimitiveMobility ScenePrimitive::GetMobility()
	{
		return Mobility;
	}

	int32 ScenePrimitive::GetLayer()
	{
		return Layer;
	}

	Ry::String ScenePrimitive::GetPipelineId()
	{
		return PipelineId;
	}

	Texture* ScenePrimitive::GetTexture()
	{
		return nullptr;
	}

	TextureScenePrimitive::TextureScenePrimitive(PrimitiveMobility Mobility, const TextureRegion& Region):
	ScenePrimitive(Mobility)
	{
		this->Texture = Region;
		Item = MakeItem();
		ItemSet->Items.Add(Item);
	}

	Texture* TextureScenePrimitive::GetTexture()
	{
		return Texture.Parent;
	}

	void TextureScenePrimitive::Draw(float X, float Y, float W, float H, float Ox, float Oy, float Rotation)
	{

		Ry::BatchTexture(Item, WHITE, X, Y,
			Texture.GetUx(),
			Texture.GetVy(),
			Texture.GetUw(),
			Texture.GetVh(),
			Ox,
			Oy,
			W,
			H,
			0.0f);
	}

	Texture* AnimationScenePrimitive::GetTexture()
	{
		return nullptr;
	}

	void AnimationScenePrimitive::Draw(float X, float Y, float W, float H, float Ox, float Oy, float Rotation)
	{
		
	}

	Scene2D::Scene2D(Ry::SwapChain* Parent)
	{
		this->SC = Parent;
		
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);

		// Create batches
		DynamicBatch = new Batch(Parent, Parent->GetDefaultRenderPass());
		StaticBatch  = new Batch(Parent, Parent->GetDefaultRenderPass());
	}

	void Scene2D::AddPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive)
	{
		if(Primitive->GetMobility() == Ry::PrimitiveMobility::Movable)
		{
			// Dynamic batch, primitive's position can update on the fly
			AddDynamicPrimitive(Primitive);
		}
		else if (Primitive->GetMobility() == Ry::PrimitiveMobility::Static)
		{
			// Static batch, primitive's position won't be updated
			AddStaticPrimitive(Primitive);
		}

	}

	void Scene2D::RemovePrimitive(Ry::SharedPtr<ScenePrimitive> Primitive)
	{
		if (Primitive->GetMobility() == Ry::PrimitiveMobility::Static)
		{
			RemoveStaticPrimitive(Primitive);
		}
		else if (Primitive->GetMobility() == Ry::PrimitiveMobility::Movable)
		{
			RemoveDynamicPrimitive(Primitive);
		}
	}

	void Scene2D::AddDynamicPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive)
	{
		PipelineState State;
		DynamicBatch->AddItemSet(Primitive->GetItemSet(), Primitive->GetPipelineId(), State, Primitive->GetTexture(), Primitive->GetLayer());
		DynamicBatch->Update();
	}

	void Scene2D::RemoveDynamicPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive)
	{
		DynamicBatch->RemoveItemSet(Primitive->GetItemSet());
	}

	void Scene2D::AddStaticPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive)
	{
		PipelineState State;
		StaticBatch->AddItemSet(Primitive->GetItemSet(), Primitive->GetPipelineId(), State, Primitive->GetTexture(), Primitive->GetLayer());

		StaticBatch->Update(); // Immediately update static batch
	}

	void Scene2D::RemoveStaticPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive)
	{
		StaticBatch->RemoveItemSet(Primitive->GetItemSet());

		StaticBatch->Update(); // Immediately update static batch
	}

	void Scene2D::Update(float Delta)
	{
		// Update dynamic primitives and dynamic batch

		DynamicBatch->Update();
	}

	void Scene2D::Render()
	{
		// Render batches
		DynamicBatch->Render();
		StaticBatch->Render();

		// Record commands
		RecordCommands();

		// Submit command buffer
		Cmd->Submit();
	}

	void Scene2D::Resize(int32 Width, int32 Height)
	{		
		// Update batches
		StaticBatch->SetRenderPass(SC->GetDefaultRenderPass());
		DynamicBatch->SetRenderPass(SC->GetDefaultRenderPass());

		StaticBatch->Resize(Width, Height);
		DynamicBatch->Resize(Width, Height);
	}

	void Scene2D::RecordCommands()
	{
		Cmd->Reset();

		Cmd->BeginCmd();
		{
			Cmd->BeginRenderPass(SC->GetDefaultRenderPass());
			{
				int32 Layer = 0;
			
				while(Layer < StaticBatch->GetLayerCount() && Layer < DynamicBatch->GetLayerCount())
				{
					// Draw static and dynamic batches
					CommandBuffer* StaticBatBuffer  = StaticBatch->GetCommandBuffer(Layer);
					CommandBuffer* DynamicBatBuffer = DynamicBatch->GetCommandBuffer(Layer);

					if (DynamicBatBuffer)
						Cmd->DrawCommandBuffer(DynamicBatBuffer);
					if (StaticBatBuffer)
						Cmd->DrawCommandBuffer(StaticBatBuffer);

					Layer++;
				}

				while (Layer < StaticBatch->GetLayerCount())
				{
					// Draw static and dynamic batches
					CommandBuffer* StaticBatBuffer = StaticBatch->GetCommandBuffer(Layer);
					if (StaticBatBuffer)
						Cmd->DrawCommandBuffer(StaticBatBuffer);
					Layer++;
				}

				while (Layer < DynamicBatch->GetLayerCount())
				{
					// Draw static and dynamic batches
					CommandBuffer* DynamicBatBuffer = DynamicBatch->GetCommandBuffer(Layer);
					if (DynamicBatBuffer)
						Cmd->DrawCommandBuffer(DynamicBatBuffer);
					Layer++;
				}
			}
			Cmd->EndRenderPass();
		}
		Cmd->EndCmd();
	}

	void Scene2D::OnItemSetDirty(Ry::ScenePrimitive* Prim)
	{
		Batch* Bat = nullptr;
		if(Prim->GetMobility() == Ry::PrimitiveMobility::Movable)
			Bat = DynamicBatch;
		else if (Prim->GetMobility() == Ry::PrimitiveMobility::Static)
			Bat = StaticBatch;

		if (Bat)
		{
			PipelineState State;
			Bat->RemoveItemSet(Prim->GetItemSet());
			Bat->AddItemSet(Prim->GetItemSet(), Prim->GetPipelineId(), State, Prim->GetTexture(), Prim->GetLayer());
		}

	}

	
}

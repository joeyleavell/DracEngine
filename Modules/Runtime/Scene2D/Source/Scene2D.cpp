#include "Scene2D.h"
#include "Interface/RenderAPI.h"
#include "SwapChain.h"
#include "Interface/RenderCommand.h"
#include "Timer.h"
#include "Camera.h"
#include "Font.h"

namespace Ry
{
	ScenePrimitive2D::ScenePrimitive2D(PrimitiveMobility Mobility)
	{
		ItemSet = MakeItemSet();
		PipelineId = "Texture";
		this->Mobility = Mobility;
	}

	Ry::SharedPtr<BatchItemSet> ScenePrimitive2D::GetItemSet()
	{
		return ItemSet;
	}

	PrimitiveMobility ScenePrimitive2D::GetMobility()
	{
		return Mobility;
	}

	int32 ScenePrimitive2D::GetLayer()
	{
		return Layer;
	}

	Ry::String ScenePrimitive2D::GetPipelineId()
	{
		return PipelineId;
	}

	void ScenePrimitive2D::SetLayer(int32 Layer)
	{
		this->Layer = Layer;
		bPrimitiveStateDirty = true;
	}

	Texture* ScenePrimitive2D::GetTexture()
	{
		return nullptr;
	}

	QuadScenePrimitive::QuadScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size):
	ScenePrimitive2D(Mobility)
	{
		this->Size = Size;
	}

	TextScenePrimitive::TextScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, Ry::String Text, BitmapFont* Font):
	ScenePrimitive2D(Mobility)
	{
		this->Width = Size.x;
		this->Font = Font;
		this->PipelineId = "Font";
		ComputeTextData(TextData, Text);
	}

	Texture* TextScenePrimitive::GetTexture()
	{
		return Font->GetAtlasTexture();
	}

	void TextScenePrimitive::Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin)
	{
		Ry::BatchText(ItemSet, WHITE, 
			Font, 
			TextData, 
			Transform[0][2], Transform[1][2], 
			Width
		);
	}

	TextureScenePrimitive::TextureScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, const TextureRegion& Region):
	QuadScenePrimitive(Mobility, Size)
	{
		this->Texture = Region;

		Item = MakeItem();
		ItemSet->Items.Add(Item);
	}

	Texture* TextureScenePrimitive::GetTexture()
	{
		return Texture.Parent;
	}

	void TextureScenePrimitive::Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin)
	{

		Ry::BatchTextureTransform(Item, WHITE, Transform,
			Texture.GetUx(),
			Texture.GetVy(),
			Texture.GetUw(),
			Texture.GetVh(),
			Origin.x,
			Origin.y,
			Size.x,
			Size.y,
			0.0f);
	}

	RectScenePrimitive::RectScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, Color RectColor):
	QuadScenePrimitive(Mobility, Size)
	{
		PipelineId = "Shape";
		
		this->RectColor = RectColor;
		Item = MakeItem();
	}

	Texture* RectScenePrimitive::GetTexture()
	{
		return nullptr;
	}

	void RectScenePrimitive::Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin)
	{
		ItemSet->Items.Clear();
		ItemSet->AddItem(Item);

		Item->Clear();

		Ry::BatchRectangleTransform(Item, RectColor, Transform,
			Size.x, Size.y,
			Origin.x, Origin.y,
			0.0f);
	}

	AnimationScenePrimitive::AnimationScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, Ry::SharedPtr<Animation> Anim):
	QuadScenePrimitive(Mobility, Size)
	{
		this->Anim = Anim;
		this->FrameIndex = 0;

		AnimTimer = new Timer;

		if(Anim.IsValid())
		{
			AnimTimer->set_delay(Anim->GetSpeed());
		}

		// Create animation item
		Item = MakeItem();
		ItemSet->Items.Add(Item);

		this->bPlaying = true;
	}

	AnimationScenePrimitive::~AnimationScenePrimitive()
	{
		delete AnimTimer;
	}

	Texture* AnimationScenePrimitive::GetTexture()
	{
		return Anim->GetParent();
	}

	void AnimationScenePrimitive::Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin)
	{
		if (Anim->GetNumFrames() <= 0)
			return;
		
		if(AnimTimer->is_ready())
		{
			FrameIndex++;
		}

		FrameIndex = FrameIndex % Anim->GetNumFrames();

		TextureRegion& CurFrame = Anim->GetFrame(FrameIndex);

		Ry::BatchTextureTransform(Item, WHITE, Transform,
			CurFrame.GetUx(),
			CurFrame.GetVy(),
			CurFrame.GetUw(),
			CurFrame.GetVh(),
			Origin.x,
			Origin.y,
			Size.x,
			Size.y,
			0.0f);
	}

	void AnimationScenePrimitive::Pause(int32 AtFrame)
	{
		this->bPlaying = false;
		this->FrameIndex = AtFrame;
	}

	void AnimationScenePrimitive::Play()
	{
		this->bPlaying = true;
	}

	void AnimationScenePrimitive::SetAnim(SharedPtr<Animation> Anim)
	{
		this->Anim = Anim;
		if (Anim.IsValid())
		{
			AnimTimer->set_delay(Anim->GetSpeed());
		}
	}

	void AnimationScenePrimitive::SetDelay(float Delay)
	{
		AnimTimer->set_delay(Delay);
	}

	Scene2D::Scene2D(Ry::SwapChain* Parent)
	{
		this->SC = Parent;
		
		Cmd = Ry::RendAPI->CreateCommandBuffer(Parent);

		// Create batches
		DynamicBatch = new Batch(Parent, Parent->GetDefaultRenderPass());
		StaticBatch  = new Batch(Parent, Parent->GetDefaultRenderPass());
	}

	void Scene2D::AddPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive)
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

	void Scene2D::RemovePrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive)
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

	void Scene2D::AddDynamicPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive)
	{
		PipelineState State;
		DynamicBatch->AddItemSet(Primitive->GetItemSet(), Primitive->GetPipelineId(), State, Primitive->GetTexture(), Primitive->GetLayer());
		DynamicBatch->Update();
	}

	void Scene2D::RemoveDynamicPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive)
	{
		DynamicBatch->RemoveItemSet(Primitive->GetItemSet());
	}

	void Scene2D::AddStaticPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive)
	{
		PipelineState State;
		StaticBatch->AddItemSet(Primitive->GetItemSet(), Primitive->GetPipelineId(), State, Primitive->GetTexture(), Primitive->GetLayer());

		StaticBatch->Update(); // Immediately update static batch
	}

	void Scene2D::RemoveStaticPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive)
	{
		StaticBatch->RemoveItemSet(Primitive->GetItemSet());

		StaticBatch->Update(); // Immediately update static batch
	}

	void Scene2D::Update(float Delta)
	{
		// Update dynamic primitives and dynamic batch

		DynamicBatch->Update();
	}

	void Scene2D::Render(Ry::Camera2D& Cam)
	{
		DynamicBatch->Camera(&Cam);
		StaticBatch->Camera(&Cam);

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

	void Scene2D::UpdateStatic()
	{
		StaticBatch->Update();
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

	void Scene2D::OnItemSetDirty(Ry::ScenePrimitive2D* Prim)
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

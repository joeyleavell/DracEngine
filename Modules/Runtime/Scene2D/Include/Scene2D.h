#pragma once

#include "Data/ArrayList.h"
#include "Core/Memory/SharedPtr.h"
#include "Scene2D.gen.h"
#include "2D/Batch/Batch.h"
#include "Animation.h"

namespace Ry
{

	class Timer;

	enum PrimitiveMobility
	{
		Static,
		Movable
	};

	class SCENE2D_MODULE ScenePrimitive2D
	{
	public:

		Ry::MulticastDelegate<ScenePrimitive2D*> OnItemSetDirty;

		ScenePrimitive2D(PrimitiveMobility Mobility);

		Ry::SharedPtr<BatchItemSet> GetItemSet();

		PrimitiveMobility GetMobility();
		int32 GetLayer();
		Ry::String GetPipelineId();
		
		virtual Texture* GetTexture();
		virtual void Draw(float X, float Y, float W, float H, float Ox, float Oy, float Rotation) = 0;

	protected:

		Ry::SharedPtr<BatchItemSet> ItemSet;

		Ry::String PipelineId;

		int32 Layer = 0;

		PrimitiveMobility Mobility;


	};

	class SCENE2D_MODULE TextureScenePrimitive : public ScenePrimitive2D
	{
	public:

		TextureScenePrimitive(PrimitiveMobility Mobility, const TextureRegion& Region);

		Texture* GetTexture() override;

		void Draw(float X, float Y, float W, float H, float Ox, float Oy, float Rotation) override;

	private:

		Ry::SharedPtr<BatchItem> Item;

		TextureRegion Texture;

	};

	class SCENE2D_MODULE AnimationScenePrimitive : public ScenePrimitive2D
	{
	public:

		AnimationScenePrimitive(PrimitiveMobility Mobility, Ry::SharedPtr<Animation> Anim);
		~AnimationScenePrimitive();

		Texture* GetTexture() override;

		void Draw(float X, float Y, float W, float H, float Ox, float Oy, float Rotation) override;

		// todo: implement
		void SetAnim() {};

	private:
		int32 FrameIndex;
		Ry::SharedPtr<Animation> Anim;
		Timer* AnimTimer;

		Ry::SharedPtr<BatchItem> Item;
	};

	class SCENE2D_MODULE Scene2D
	{
	public:

		Scene2D(Ry::SwapChain* Parent);

		void AddPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void RemovePrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);

		void Update(float Delta);

		void Render();

		void Resize(int32 Width, int32 Height);

	private:

		Ry::SwapChain* SC;

		Ry::CommandBuffer* Cmd;

		void RecordCommands();

		void OnItemSetDirty(Ry::ScenePrimitive2D* Prim);

		void AddDynamicPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void RemoveDynamicPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);

		void AddStaticPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void RemoveStaticPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);

		Ry::ArrayList<Ry::SharedPtr<ScenePrimitive2D>> StaticPrimitives;
		Ry::ArrayList<Ry::SharedPtr<ScenePrimitive2D>> DynamicPrimitives;

		Ry::Batch* StaticBatch;
		Ry::Batch* DynamicBatch; 
		
	};

	
}

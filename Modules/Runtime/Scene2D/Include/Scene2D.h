#pragma once

#include "Entity2D.h"
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

	class SCENE2D_MODULE ScenePrimitive
	{
	public:

		Ry::MulticastDelegate<ScenePrimitive*> OnItemSetDirty;

		ScenePrimitive(PrimitiveMobility Mobility);

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

	class SCENE2D_MODULE TextureScenePrimitive : public ScenePrimitive
	{
	public:

		TextureScenePrimitive(PrimitiveMobility Mobility, const TextureRegion& Region);

		Texture* GetTexture() override;

		void Draw(float X, float Y, float W, float H, float Ox, float Oy, float Rotation) override;

	private:

		Ry::SharedPtr<BatchItem> Item;

		TextureRegion Texture;

	};

	class SCENE2D_MODULE AnimationScenePrimitive : public ScenePrimitive
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

		void AddPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive);
		void RemovePrimitive(Ry::SharedPtr<ScenePrimitive> Primitive);

		void Update(float Delta);

		void Render();

		void Resize(int32 Width, int32 Height);

	private:

		Ry::SwapChain* SC;

		Ry::CommandBuffer* Cmd;

		void RecordCommands();

		void OnItemSetDirty(Ry::ScenePrimitive* Prim);

		void AddDynamicPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive);
		void RemoveDynamicPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive);

		void AddStaticPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive);
		void RemoveStaticPrimitive(Ry::SharedPtr<ScenePrimitive> Primitive);

		Ry::ArrayList<Ry::SharedPtr<ScenePrimitive>> StaticPrimitives;
		Ry::ArrayList<Ry::SharedPtr<ScenePrimitive>> DynamicPrimitives;

		Ry::Batch* StaticBatch;
		Ry::Batch* DynamicBatch; 
		
	};

	
}

#pragma once

#include "Data/Map.h"
#include "Algorithm/Algorithm.h"
#include "Data/ArrayList.h"
#include "Core/Memory/SharedPtr.h"
#include "Scene2D.gen.h"
#include "2D/Batch/Batch.h"
#include "Animation.h"
#include "Transform.h"

namespace Ry
{
	class Camera2D;
	class OrthoCamera;

	class Timer;

	enum PrimitiveMobility
	{
		Static,
		Movable
	};

	struct Particle
	{
		Ry::Transform2D Transform;
		float Vx, Vy;
		float Width;
		float Height;
		float Opacity;
		Color Tint;
		TextureRegion Texture;

		float TimeAlive;
	};

	class SCENE2D_MODULE ScenePrimitive2D
	{
	public:

		bool bItemSetDirty;
		bool bPrimitiveStateDirty;

		ScenePrimitive2D(PrimitiveMobility Mobility);
		virtual ~ScenePrimitive2D() {};

		Ry::SharedPtr<BatchItemSet> GetItemSet();

		PrimitiveMobility GetMobility();
		int32 GetLayer();
		Ry::String GetPipelineId();

		void SetLayer(int32 Layer);
		
		virtual Texture* GetTexture();
		virtual void Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin) = 0;

	protected:

		Ry::SharedPtr<BatchItemSet> ItemSet;

		Ry::String PipelineId;

		int32 Layer = 0;

		PrimitiveMobility Mobility;


	};

	class SCENE2D_MODULE QuadScenePrimitive : public ScenePrimitive2D
	{
	public:

		QuadScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size);

	protected:

		Ry::Vector2 Size;
	};

	class SCENE2D_MODULE ParticleEmitterPrimitive : public ScenePrimitive2D
	{
	public:

		ParticleEmitterPrimitive(PrimitiveMobility Mobility, Texture* ParticleTexture);

		Texture* GetTexture() override;

		void Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin) override;

		void AddParticle(SharedPtr<Particle> Part);
		void RemoveParticle(SharedPtr<Particle> Part);

	private:

		Texture* ParticleTexture = nullptr;

		// Pool of batch items for particles to use
		Ry::ArrayList<Ry::SharedPtr<BatchItem>> FreeItems;

		Ry::OAHashMap<Particle*, Ry::SharedPtr<BatchItem>> Particles;

	};

	class SCENE2D_MODULE TextScenePrimitive : public ScenePrimitive2D
	{
	public:

		TextScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, Ry::String Text, BitmapFont* Font);

		Texture* GetTexture() override;

		void Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin) override;

	private:

		float Width;
		PrecomputedTextData TextData;
		BitmapFont* Font;
	};

	class SCENE2D_MODULE TextureScenePrimitive : public QuadScenePrimitive
	{
	public:

		TextureScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, const TextureRegion& Region);

		Texture* GetTexture() override;

		void Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin) override;

	private:

		Ry::SharedPtr<BatchItem> Item;

		TextureRegion Texture;

	};

	class SCENE2D_MODULE RectScenePrimitive : public QuadScenePrimitive
	{
	public:

		RectScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, Color RectColor);

		Texture* GetTexture() override;

		void Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin) override;

	private:

		Color RectColor;

		Ry::SharedPtr<BatchItem> Item;

		TextureRegion Texture;

	};

	class SCENE2D_MODULE AnimationScenePrimitive : public QuadScenePrimitive
	{
	public:

		AnimationScenePrimitive(PrimitiveMobility Mobility, Ry::Vector2 Size, Ry::SharedPtr<Animation> Anim);
		~AnimationScenePrimitive();

		Texture* GetTexture() override;

		void Draw(Ry::Matrix3 Transform, Ry::Vector2 Origin) override;

		void Pause(int32 AtFrame);
		void Play();
		void SetAnim(SharedPtr<Animation> Anim);
		void SetDelay(float Delay);

	private:
		bool bPlaying;
		int32 FrameIndex;
		Ry::SharedPtr<Animation> Anim;
		Timer* AnimTimer;

		Ry::SharedPtr<BatchItem> Item;
	};

	class SCENE2D_MODULE Scene2D
	{
	public:

		MulticastDelegate<Ry::SharedPtr<ScenePrimitive2D>> OnPrimitiveAdded;
		MulticastDelegate<Ry::SharedPtr<ScenePrimitive2D>> OnPrimitiveRemoved;

		Scene2D(Ry::SwapChain* Parent);

		void AddPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void RemovePrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);

	private:

		//void OnItemSetDirty(Ry::ScenePrimitive2D* Prim);

		Ry::ArrayList<Ry::SharedPtr<ScenePrimitive2D>> StaticPrimitives;
		Ry::ArrayList<Ry::SharedPtr<ScenePrimitive2D>> DynamicPrimitives;

	};

	class Scene2DRenderer
	{
	public:

		Scene2DRenderer(Scene2D* Target, SwapChain* ParentSC);

		void Update(float Delta);
		void Render(Ry::Camera2D& Cam);
		void AddPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void RemovePrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void Resize(int32 Width, int32 Height);

		void AddCustomBatch(Batch* Batch);

		void UpdateStatic();

	private:

		void AddDynamicPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void RemoveDynamicPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);

		void AddStaticPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);
		void RemoveStaticPrimitive(Ry::SharedPtr<ScenePrimitive2D> Primitive);

		void RecordCommands();

		Scene2D* TargetScene;

		Ry::ArrayList<Batch*> AllBatches;

		Ry::SwapChain* SC;

		Ry::CommandBuffer* Cmd;

		Ry::Batch* StaticBatch;
		Ry::Batch* DynamicBatch;

		Ry::ArrayList<Batch*> CustomBatches;
		
	};
}

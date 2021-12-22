#pragma once

#include "Scene2D.h"
#include "Widget/Layout/SlotWidget.h"
#include "SceneView.gen.h"

namespace Ry
{

	class EDITORMAIN_MODULE SceneView : public Ry::SlotWidget
	{
	public:
		GeneratedBody()

		MulticastDelegate<uint32, uint32> OnSceneResized;

		SceneView();

		void Draw() override;
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		SizeType ComputeSize() const override;

		void SetSceneRenderer(Scene2DRenderer* Rend);

	private:

		void UpdateSceneRenderer(SizeType Size);

		// The renderer used to provide the scene texture for the widget. This widget will handle resizing this renderer.
		Scene2DRenderer* Renderer{};

		// Keep track of batches this scene view is in, when we resize we have to update the data
		Ry::OASet<Batch*> BatchesShown;

		int32 LastWidth = -1;
		int32 LastHeight = -1;

		Ry::SharedPtr<BatchItem> SceneTexture;

	} RefClass();

}

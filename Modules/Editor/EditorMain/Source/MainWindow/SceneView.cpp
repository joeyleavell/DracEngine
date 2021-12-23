#include "MainWindow/SceneView.h"

namespace Ry
{
	SceneView::SceneView()
	{
		SceneTexture = MakeItem();
	}

	void SceneView::Draw()
	{
		SlotWidget::Draw();

		if(Renderer)
		{
			Point Abs = GetAbsolutePosition();
			SizeType Size = Widget::GetScaledSlotSize(this);

			if (Size.Width != LastWidth || Size.Height != LastHeight)
			{
				UpdateSceneRenderer(Size);

				// Broadcast scene size changed
				OnSceneResized.Broadcast(Size.Width, Size.Height);
			}

			Ry::BatchTexture(SceneTexture,
				WHITE,
				(float)Abs.X, (float)Abs.Y,
				0.0, 1.0f,
				1.0f, -1.0f,
				0.0f, 0.0f,
				Size.Width, Size.Height,
				0.0f
			);
		}
	}

	void SceneView::OnShow(Ry::Batch* Batch)
	{
		SlotWidget::OnShow(Batch);

		if(Renderer)
		{
			Batch->AddItem(SceneTexture,
				"Texture",
				Widget::GetPipelineState(this),
				Renderer->GetSceneTexture(),
				WidgetLayer
			);

			BatchesShown.Insert(Batch);
		}
	}

	void SceneView::OnHide(Ry::Batch* Batch)
	{
		SlotWidget::OnHide(Batch);

		Batch->RemoveItem(SceneTexture);

		BatchesShown.Remove(Batch);
	}

	SizeType SceneView::ComputeSize() const
	{
		// For now, scene view always occupies the space of the parent
		return Widget::GetScaledSlotSize(this);
	}

	void SceneView::UpdateSceneRenderer(SizeType Size)
	{
		// Need to resize the scene renderer to match new size in viewport
		Renderer->Resize(Size.Width, Size.Height);
		//Renderer->Resize(300, 300);

		// Refresh the data in the batches shown
		for (Batch* Batch : BatchesShown)
		{
			Batch->RemoveItem(SceneTexture);
			Batch->AddItem(SceneTexture,
				"Texture",
				Widget::GetPipelineState(this),
				Renderer->GetSceneTexture(),
				WidgetLayer
			);
		}

		// Updated the cached size
		LastWidth = Size.Width;
		LastHeight = Size.Height;
	}

	void SceneView::SetSceneRenderer(Scene2DRenderer* Rend)
	{
		this->Renderer = Rend;

		// Update renderer
		UpdateSceneRenderer(ComputeSize());
	}

}

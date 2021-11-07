#pragma once

#include "Widget/Panel.h"
#include "2D/Batch/Batch.h"

namespace Ry
{

	enum class SplitterType
	{
		VERTICAL,
		HORIZONTAL
	};

    class UI_MODULE Splitter : public Ry::PanelWidget
    {
    public:

        SplitterType Type;
		float BarThickness;

    	/*
    	 * Determines what percentage the bar is along at. In the range [0, 1].
    	 */
		float BarPosition;

		WidgetBeginArgsSlot(Splitter)
			WidgetProp(SplitterType, Type)
			WidgetProp(float, BarThickness)
		WidgetEndArgs()

		Splitter() {};

		struct Slot : public PanelWidget::Slot
		{
			Slot() :
				PanelWidget::Slot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
				PanelWidget::Slot(Wid)
			{
			}
		};

		static Splitter::Slot MakeSlot()
		{
			Splitter::Slot NewSlot;
			return NewSlot;
		}

		void Construct(Splitter::Args& In)
		{
			CORE_ASSERT(In.Slots.GetSize() <= 2);

			this->Type = In.mType;
			this->BarThickness = In.mBarThickness;
			this->BarPosition = 0.5f;

			for (PanelWidget::Slot& Child : In.Slots)
			{
				SharedPtr<Slot> Result = CastShared<Slot>(AppendSlot(Child.GetWidget()));
				Result->SetPadding(Child.GetPadding());
			}

			SplitterItem = MakeItem();
		}

    	RectScissor GetClipSpace(const Widget* ForWidget) const override
		{
			// Default nullptr (entire widget) clip space
			if (!ForWidget)
				return Widget::GetClipSpace(ForWidget);

			RectScissor OurSize = Widget::GetClipSpace(this);
			Point OurPos = GetAbsolutePosition();

			if(Type == SplitterType::HORIZONTAL)
			{
				float SplitterWidth = OurSize.Width * BarPosition;

				if (ForWidget == Children[0].Get())
				{
					// Clip space for left widget
					return RectScissor{ OurPos.X, OurPos.Y, (int32) SplitterWidth - (int32)(BarThickness / 2.0f + 1.0f), OurSize.Height };
				}
				else
				{
					// Clip space for right widget
					return RectScissor{ OurPos.X + (int32) SplitterWidth + (int32)(BarThickness / 2.0f), OurPos.Y, OurSize.Width - (int32)SplitterWidth - (int32)(BarThickness / 2.0f), OurSize.Height };
				}
			}
			else // Vertical
			{
				float SplitterHeight = OurSize.Height * BarPosition;

				if (ForWidget == Children[0].Get())
				{
					// Clip space for bottom widget
					return RectScissor{ OurPos.X, OurPos.Y, OurSize.Width, (int32) SplitterHeight - (int32) (BarThickness / 2.0f + 1.0f)};
				}
				else
				{
					// Clip space for top widget
					return RectScissor{ OurPos.X, OurPos.Y + (int32)SplitterHeight + (int32)(BarThickness / 2.0f), OurSize.Width, OurSize.Height - (int32)SplitterHeight - (int32)(BarThickness / 2.0f) };
				}

			}
			
		}

    	void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates) override
		{
			OutStates.Add(GetPipelineState(Children[0].Get()));
			OutStates.Add(GetPipelineState(Children[1].Get()));
			OutStates.Add(GetPipelineState(nullptr));
		}

		PipelineState GetPipelineState(Widget* ForWidget) const override
		{
			// Default pipeline state
			if (!ForWidget)
				return Widget::GetPipelineState(ForWidget);

			PipelineState State;
			State.Scissor = GetClipSpace(ForWidget);

			// Must have two children
			if(ForWidget == Children[0].Get())
			{
				State.StateID = Ry::to_string(GetWidgetID()) + "_A";
			}
			else
			{
				State.StateID = Ry::to_string(GetWidgetID()) + "_B";
			}

			return State;
		}

    	SharedPtr<PanelWidget::Slot> AppendSlot(SharedPtr<Widget> Widget) override;

    	void OnShow(Ry::Batch* Batch) override;
    	void OnHide(Ry::Batch* Batch) override;

    	SizeType ComputeSize() const override;
        void Arrange() override;
		void Draw(StyleSet* Style) override;

    	bool OnMouseEvent(const MouseEvent& MouseEv) override;
    	bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;

    private:

		bool bDragging = false;

		Ry::SharedPtr<Ry::BatchItem> SplitterItem;


    };

}
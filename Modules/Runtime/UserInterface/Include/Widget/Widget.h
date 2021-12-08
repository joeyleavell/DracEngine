#pragma once

#include "Core/Core.h"
#include "Core/Delegate.h"
#include "Event.h"
#include "Interface/Pipeline.h"
#include "2D/Batch/Batch.h"
#include "Core/Object.h"
#include "Widget.gen.h"

namespace Ry
{
	struct Event;

	class Batch;
	class StyleSet;

	struct Margin
	{
		float Left;
		float Right;
		float Top;
		float Bottom;

		Margin()
		{
			this->Left = Right = Top = Bottom = 0.0f;
		}

		void Set(float Horizontal, float Vertical)
		{
			this->Left = Right = Horizontal;
			this->Top = Bottom = Vertical;
		}

		Margin& operator=(float Pad)
		{
			this->Left = Right = Top = Bottom = Pad;
			return *this;
		}
		
	};

	template<typename T>
	struct OptionalValue
	{
		OptionalValue()
		{
			bSet = false;
		}

		OptionalValue(T Val)
		{
			this->Value = Val;
			bSet = true;
		}

		bool IsSet()
		{
			return bSet;
		}

		void Set(const T& Value)
		{
			this->Value = Value;
			bSet = true;
		}

		void Set(T&& Value)
		{
			this->Value = Value;
			bSet = true;
		}

		T Get()
		{
			return Value;
		}

		operator T() const
		{
			return Value;
		}

		OptionalValue<T>& operator=(T&& Value)
		{
			Set(std::move(Value));
			return *this;
		}

		OptionalValue<T>& operator=(const OptionalValue<T>& Opt)
		{
			this->bSet = Opt.bSet;
			this->Value = Opt.Value;
			return *this;
		}

		OptionalValue<T>& operator=(const T& Value)
		{
			Set(Value);
			return *this;
		}
		
	private:
		T Value;
		mutable bool bSet;
		
	};
	
	struct USERINTERFACE_MODULE SizeType
	{
		int32 Width;
		int32 Height;

		SizeType()
		{
			Width = 0;
			Height = 0;
		}

		SizeType(int32 W, int32 H)
		{
			Width = W;
			Height = H;
		}
	};

	struct USERINTERFACE_MODULE Point
	{
		int32 X;
		int32 Y;

		Point()
		{
			X = 0;
			Y = 0;
		}

		Point(int32 X, int32 Y)
		{
			this->X = X;
			this->Y = Y;
		}
	};

	// TODO: be able to reflect enums?
	
	const uint8 SIZE_MODE_AUTO = 0;
	const uint8 SIZE_MODE_PERCENTAGE = 1;

	const uint8 VERT_TOP_ALIGN = 0;
	const uint8 VERT_CENTER_ALIGN = 1;
	const uint8 VERT_BOTTOM_ALIGN = 2;

	const uint8 HOR_LEFT_ALIGN = 0;
	const uint8 HOR_CENTER_ALIGN = 1;
	const uint8 HOR_RIGHT_ALIGN = 2;

	class USERINTERFACE_MODULE Widget : public Ry::Object
	{
	public:

		GeneratedBody()

		/**
		 * A string that uniquely identifies this widget within a hierarchy. Must be unique.
		 */
		RefField()
		Ry::String Id;

		/**
		 * The group of this widget. Does not need to be unique, used to group widgets together.
		 */
		RefField()
		Ry::String Class;


		/**
		 * Delegates
		 *
		 * @param Ry::Widget* The widget whose render state is dirty
		 */
		MulticastDelegate<Ry::Widget*, bool> RenderStateDirty;

		Widget();
		
		virtual ~Widget() = default;

		const Ry::String& GetId() const;
		const Ry::String& GetClassName() const;
		const Ry::StyleSet* GetStyle() const;
		bool IsHovered();
		bool IsPressed();
		bool IsVisible();

		Point GetRelativePosition() const;
		Point GetAbsolutePosition() const;
		Widget& SetRelativePosition(float X, float Y);
		Widget& SetMaxSize(int32 MaxWidth, int32 MaxHeight);
		void MarkDirty(Widget* Self, bool bFullRefresh = false);
		void SetVisible(bool bVisibility, bool bPropagate);

		virtual void SetVisibleInternal(bool bVisibility, bool bPropagate);
		virtual SharedPtr<Widget> FindChildWidgetById(const Ry::String& Id) const { return Ry::SharedPtr<Widget>(); };
		virtual void SetId(const Ry::String& Id);
		virtual void SetClass(const Ry::String& Class);
		virtual void SetStyle(const Ry::StyleSet* Style);
		virtual void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren);
		virtual void SetParent(Widget* Parent);
		virtual PipelineState GetPipelineState(const Widget* ForWidget) const;
		virtual RectScissor GetClipSpace(const Widget* ForWidget) const;
		virtual Widget& operator[](SharedPtr<Ry::Widget> Child);
		virtual void Arrange();
		virtual void OnHovered(const MouseEvent& MouseEv);
		virtual void OnUnhovered(const MouseEvent& MouseEv);
		virtual bool OnPressed(const MouseButtonEvent& MouseEv);
		virtual bool OnReleased(const MouseButtonEvent& MouseEv);
		virtual bool OnMouseEvent(const MouseEvent& MouseEv);
		virtual bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv);
		virtual bool OnMouseClicked(const MouseClickEvent& MouseEv);
		virtual bool OnMouseDragged(const MouseDragEvent& MouseEv);
		virtual bool OnMouseScroll(const MouseScrollEvent& MouseEv);
		virtual bool OnKey(const KeyEvent& KeyEv);
		virtual bool OnChar(const CharEvent& CharEv);
		virtual bool OnPathDrop(const PathDropEvent& PathDropEv);
		virtual bool OnEvent(const Event& Ev);
		virtual void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates);

		virtual SizeType GetScaledSlotSize(const Widget* ForWidget) const;

		virtual SizeType GetUnscaledSlotSize(const Widget* ForWidget) const;

		// Space that this widget would occupy without being scaled to accompany overflow
		virtual SizeType GetUnscaledOccupiedSize(const Widget* ForWidget) const;

		// Space that this widget actually ends up accupying after being scaled to account for overflow
		virtual SizeType GetScaledOccupiedSize(const Widget* ForWidget) const;

		virtual void Draw() {};
		virtual SizeType ComputeSize() const { return SizeType{}; };
		virtual void OnShow(Ry::Batch* Batch) {}
		virtual void OnHide(Ry::Batch* Batch) {};

		template <typename WidgetClass>
		SharedPtr<WidgetClass> FindChildWidget(Ry::String ChildWidgetId) const
		{
			return CastShared<WidgetClass>(FindChildWidgetById(ChildWidgetId));
		}

	protected:

		const StyleSet* Style;

		int32 GetWidgetID() const;

		SizeType CachedSize;
		Widget* Parent{};
		SizeType MaxSize;

		int32 WidgetLayer;
		
		Point RelativePosition;

		bool bHovered;
		bool bPressed;
		bool bVisible;

	private:

		uint32 WidgetID;

	} RefClass();

}

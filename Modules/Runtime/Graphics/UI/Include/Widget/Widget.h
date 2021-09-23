#pragma once

#include "Core/Core.h"
#include "Core/Delegate.h"
#include "UIGen.h"
#include "Event.h"
#include "Interface/Pipeline.h"
#include "2D/Batch/Batch.h"

#define WidgetBeginArgsSlot(ClassName) \
public: \
struct Args \
{ \
	typedef ClassName::Args WidgetArgsType; \
	Ry::ArrayList<SharedPtr<Widget>> Children; \
	Ry::ArrayList<ClassName::Slot> Slots; \
	WidgetArgsType& operator [] (Ry::SharedPtr<Widget> Wid) \
	{ \
		Children.Add(Wid); \
		return *this; \
	} \
	WidgetArgsType& operator+(ClassName::Slot WidgetSlot) \
	{ \
		Slots.Add(WidgetSlot); \
		return *this; \
	}

#define WidgetBeginArgs(ClassName) \
public: \
struct Args \
{ \
	typedef ClassName::Args WidgetArgsType; \
	Ry::ArrayList<SharedPtr<Widget>> Children; \
	WidgetArgsType& operator [] (Ry::SharedPtr<Widget> Wid) \
	{ \
		Children.Add(Wid); \
		return *this; \
	} \

#define WidgetPropDefault(Type, Name, Default) \
OptionalValue<Type> m##Name = Default; \
WidgetArgsType& Name(Type InAttrib) \
{ \
	m##Name = InAttrib; \
	return *this; \
}

#define WidgetProp(Type, Name) \
OptionalValue<Type> m##Name; \
WidgetArgsType& Name(Type InAttrib) \
{ \
	m##Name = InAttrib; \
	return *this; \
}

#define WidgetEndArgs(ClassName) \
};

#define NewWidgetAssign(AssignTo, Type) Ry::WidgetDecl<Type>(AssignTo) << Type::Args()
//#define NewWidget(Type) (*(new (Type)))

#define NewWidget(Type) Ry::WidgetDecl<Type>() << Type::Args()

namespace Ry
{
	struct Event;

	class Batch;

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
	
	struct UI_MODULE SizeType
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

	struct UI_MODULE Point
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

	enum class UI_MODULE SizeMode
	{
		AUTO,
		PERCENTAGE
	};

	enum class UI_MODULE VAlign
	{
		TOP, CENTER, BOTTOM
	};

	enum class UI_MODULE HAlign
	{
		LEFT, CENTER, RIGHT
	};

	class UI_MODULE Widget
	{
	public:

		/**
		 * Delegates
		 */
		MulticastDelegate<> RenderStateDirty;

		Widget();
		
		virtual ~Widget() = default;

		void MarkDirty()
		{
			RenderStateDirty.Broadcast();
		}

		bool IsHovered()
		{
			return bHovered;
		}

		bool IsPressed()
		{
			return bPressed;
		}

		bool IsVisible()
		{
			return bVisible;
		}

		Point GetRelativePosition() const
		{
			return RelativePosition;
		}

		Point GetAbsolutePosition() const
		{
			Point AbsolutePosition = GetRelativePosition();
			if(Parent)
			{
				Point ParentPos = Parent->GetAbsolutePosition();
				AbsolutePosition.X += ParentPos.X;
				AbsolutePosition.Y += ParentPos.Y;
			}

			return AbsolutePosition;
		}

		Widget& SetRelativePosition(float X, float Y)
		{
			RelativePosition.X = (int32)X;
			RelativePosition.Y = (int32)Y;

			return *this;
		}

		Widget& SetMaxSize(int32 MaxWidth, int32 MaxHeight)
		{
			this->MaxSize = SizeType{MaxWidth, MaxHeight};

			return *this;
		}

		virtual void SetParent(Widget* Parent)
		{
			this->Parent = Parent;

			// Calculate widget layer for new parent
			int32 PreviousDepth = WidgetLayer;
			
			WidgetLayer = 0;
			Widget* Temp = Parent;
			while(Temp)
			{
				WidgetLayer++;
				Temp = Temp->Parent;
			}

			// Hide and show the widget so it's added to the correct layer
			if(WidgetLayer != PreviousDepth && IsVisible())
			{
				OnHide();
				OnShow();
			}
		}

		virtual PipelineState GetPipelineState() const
		{
			if (Parent)
			{
				// Return clip space of parent
				return Parent->GetPipelineState();
			}
			else
			{
				// By default widgets never clip
				PipelineState State;
				State.Scissor = GetClipSpace();
				State.StateID = WidgetID;

				return State;
			}
		}

		virtual RectScissor GetClipSpace() const
		{
			if(Parent)
			{
				// Return clip space of parent
				return Parent->GetClipSpace();
			}
			else
			{
				// By default widgets never clip
				static RectScissor Clip;
				return Clip;
			}
		}

		virtual Widget& operator[](SharedPtr<Ry::Widget> Child)
		{
			return *this;
		}

		virtual void SetBatch(Batch* Bat)
		{
			this->Bat = Bat;

			if(Bat && bVisible)
			{
			//	OnShow();
			}
		}

		virtual void Arrange()
		{

		}

		virtual void OnHovered(const MouseEvent& MouseEv)
		{

		}

		virtual void OnUnhovered(const MouseEvent& MouseEv)
		{

		}

		virtual bool OnPressed(const MouseButtonEvent& MouseEv)
		{
			return false;
		}

		virtual bool OnReleased(const MouseButtonEvent& MouseEv)
		{
			return false;
		}

		virtual bool OnMouseEvent(const MouseEvent& MouseEv);
		virtual bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv);
		virtual bool OnMouseClicked(const MouseClickEvent& MouseEv);
		virtual bool OnMouseDragged(const MouseDragEvent& MouseEv);
		virtual bool OnMouseScroll(const MouseScrollEvent& MouseEv);
		virtual bool OnKey(const KeyEvent& KeyEv);
		virtual bool OnChar(const CharEvent& CharEv);

		virtual bool OnEvent(const Event& Ev)
		{
			if (Ev.Type == EVENT_MOUSE)
			{
				const MouseEvent& Mouse = static_cast<const MouseEvent&>(Ev);

				return OnMouseEvent(Mouse);
			}
			else if (Ev.Type == EVENT_MOUSE_BUTTON)
			{
				const MouseButtonEvent& MouseButton = static_cast<const MouseButtonEvent&>(Ev);

				return OnMouseButtonEvent(MouseButton);
			}
			else if(Ev.Type == EVENT_MOUSE_CLICK)
			{
				const MouseClickEvent& MouseClick = static_cast<const MouseClickEvent&>(Ev);
				return OnMouseClicked(MouseClick);
			}
			else if(Ev.Type == EVENT_MOUSE_DRAG)
			{
				const MouseDragEvent& MouseDrag = static_cast<const MouseDragEvent&>(Ev);
				return OnMouseDragged(MouseDrag);
			}
			else if (Ev.Type == EVENT_MOUSE_SCROLL)
			{
				const MouseScrollEvent& MouseScroll = static_cast<const MouseScrollEvent&>(Ev);
				return OnMouseScroll(MouseScroll);
			}
			else if (Ev.Type == EVENT_KEY)
			{
				const KeyEvent& Key = static_cast<const KeyEvent&>(Ev);
				return OnKey(Key);
			}
			else if (Ev.Type == EVENT_CHAR)
			{
				const CharEvent& Char = static_cast<const CharEvent&>(Ev);
				return OnChar(Char);
			}

			return false;
		}

		virtual void SetVisible(bool bVisibility, bool bPropagate)
		{
			this->bVisible = bVisibility;

			// Check parent visibility
			if(bVisibility)
			{
				OnShow();
			}				
			else
			{
				OnHide();
			}
		}
		
		virtual void Draw() {};
		virtual SizeType ComputeSize() const { return SizeType{}; };

		virtual void OnShow() {}
		virtual void OnHide() {};

	protected:
		
		SizeType CachedSize;
		Widget* Parent;
		SizeType MaxSize;

		Ry::Batch* Bat;

		int32 WidgetLayer;

	protected:

		void UpdatePipelineState()
		{
			if(Bat)
			{
				Bat->UpdatePipelineState(GetPipelineState());
			}
		}

		/*bool IsParentHidden()
		{
			Widget* CurParent = Parent;
			bool bHidden = false;
			while(CurParent && !bHidden)
			{
				if(!CurParent->IsVisible())
				{
					bHidden = true;
				}
				CurParent = CurParent->Parent;
			}
			return bHidden;			
		}*/
		
		Point RelativePosition;

		bool bHovered;
		bool bPressed;
		bool bVisible;

		int32 GetWidgetID() const
		{
			return WidgetID;
		}

	private:

		uint32 WidgetID;

	};

	template<typename WidgetClass>
	struct WidgetDecl
	{
	private:

		Ry::SharedPtr<WidgetClass>* Dst;

	public:

		WidgetDecl()
		{
			this->Dst = nullptr;
		}

		WidgetDecl(Ry::SharedPtr<WidgetClass>& AssignTo)
		{
			this->Dst = &AssignTo;
		}

		Ry::SharedPtr<WidgetClass> operator <<(typename WidgetClass::Args ConArgs)
		{
			Ry::SharedPtr<WidgetClass> NewWidget = MakeShared(new WidgetClass);

			NewWidget->Construct(ConArgs);

			if (Dst)
			{
				(*Dst) = NewWidget;
			}

			return NewWidget;
		}
	};

}

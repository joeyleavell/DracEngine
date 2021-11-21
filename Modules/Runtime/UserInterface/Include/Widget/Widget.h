#pragma once

#include "Core/Core.h"
#include "Core/Delegate.h"
#include "Event.h"
#include "Interface/Pipeline.h"
#include "2D/Batch/Batch.h"
#include "Core/Object.h"
#include "Widget.gen.h"

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

#define WidgetEndArgs() \
};

#define NewWidgetAssign(AssignTo, Type) Ry::WidgetDecl<Type>(AssignTo) << Type::Args()
//#define NewWidget(Type) (*(new (Type)))

#define NewWidget(Type) Ry::WidgetDecl<Type>() << Type::Args()

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

	enum class SizeMode
	{
		AUTO,
		PERCENTAGE
	};

	enum class VAlign
	{
		TOP, CENTER, BOTTOM
	};

	enum class HAlign
	{
		LEFT, CENTER, RIGHT
	};

	class USERINTERFACE_MODULE Widget : public Ry::Object
	{
	public:

		GeneratedBody()

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
		virtual bool OnEvent(const Event& Ev);
		virtual void SetVisible(bool bVisibility, bool bPropagate);
		virtual void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates);
		virtual void Draw() {};
		virtual SizeType ComputeSize() const { return SizeType{}; };
		virtual void OnShow(Ry::Batch* Batch) {}
		virtual void OnHide(Ry::Batch* Batch) {};

	protected:

		const StyleSet* Style;

		int32 GetWidgetID() const;

		// Attributes used for identifying widgets from a parent
		Ry::String Id;
		Ry::String Class;

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

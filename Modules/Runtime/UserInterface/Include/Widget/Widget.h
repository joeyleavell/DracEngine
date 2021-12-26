#pragma once

#include "Core/Core.h"
#include "Core/Delegate.h"
#include "Event.h"
#include "Pipeline.h"
#include "Batch.h"
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

		SizeType(SizeType& Copy)
		{
			this->Width = Copy.Width;
			this->Height = Copy.Height;
		}

		SizeType(SizeType&& Copy) noexcept
		{
			this->Width = Copy.Width;
			this->Height = Copy.Height;
		}

		SizeType& operator=(SizeType&& Copy) noexcept
		{
			this->Width = Copy.Width;
			this->Height = Copy.Height;
			return *this;
		}

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

		friend class UserInterface;

		GeneratedBody()

		/**
		 * A string that uniquely identifies this widget within a hierarchy. This need not be a universally unique identifier, but it should be unique within a hierarchy.
		 */
		RefField()
		Ry::String Id;

		/**
		 * The class used to group this widget with other widgets within a hierarchy.
		 */
		RefField()
		Ry::String Class;

		Widget();
		virtual ~Widget() = default;

		/**
		 * @return The widget's unique identifier.
		 */
		const Ry::String& GetId() const;

		/**
		 * @return The grouping of this widget.
		 */
		const Ry::String& GetClassName() const;

		/**
		 * @return The style set for this widget.
		 */
		const Ry::StyleSet* GetStyle() const;

		/**
		 * @return Whether the mouse is hovered over this widget.
		 */
		bool IsHovered();

		/**
		 * @return Whether the mouse has the left button pressed on this widget.
		 */
		bool IsPressed();

		/**
		 * @return Whether this widget is visible.
		 */
		bool IsVisible() const;

		/**
		 * @return The position of this widget, relative to the parent widget.
		 */
		Point GetRelativePosition() const;

		/**
		 * @return The position of this widget in screen space, relative to the bottom left of the screen.
		 */
		Point GetAbsolutePosition() const;

		/**
		 * Sets the position of this widget relative to the parent/
		 *
		 * @param x,y The position of the widget
		 */
		void SetRelativePosition(float X, float Y);

		/**
		 * Sets the visibility of this widget and implicitly calls UpdateBatch().
		 *
		 * @param bVisibility	Whether the widget(s) should be visible.
		 * @param bPropagate	Whether the visibility should also propagate to the children.
		 */
		void SetVisible(bool bVisibility, bool bPropagate);

		/**
		 * Sets the batch for this widget. The batch only needs to be set at the root widget because GetBatch() will crawl up to the parent.
		 *
		 * @param Bat	The new batch
		 */
		void SetBatch(Batch* Bat);

		/**
		 * @return The batch for this widget. This will be nullptr on any non-root level widget.
		 */
		Batch* GetBatch();

		/**
		 * This function is not meant to be directly called. This should be overriden by children widgets to control what happens when a widget's visibility changes.
		 *
		 * SetVisible() is meant to be called from outside user's. That function directly calls this function.
		 */
		virtual void SetVisibleFlag(bool bVisibility, bool bPropagate);

		/**
		 * Utility to find a child widget in the hierarchy with a particular ID and automatically cast it to the templated type.
		 *
		 * @param ChildWidgetId		The ID of the child widget to search for.
		 * @return A shared ptr to the found widget which can possibly be null if no widget with the specified ID was found.
		 */
		template <typename WidgetClass>
		SharedPtr<WidgetClass> FindChildWidget(Ry::String ChildWidgetId) const
		{
			return CastShared<WidgetClass>(FindChildWidgetById(ChildWidgetId));
		}

		/**
		 * Searches this widget hierarchy for a child widget with the specified ID. This function should be overriden by children widgets that have more information about their hierarchy.
		 *
		 * @param Id	The Id to search for in the widget hierarchy.
		 */
		virtual SharedPtr<Widget> FindChildWidgetById(const Ry::String& Id) const { return Ry::SharedPtr<Widget>(); };

		/**
		 * Sets the new ID for this widget.
		 *
		 * @param Id	The new ID
		 */
		virtual void SetId(const Ry::String& Id);

		/**
		 * Sets the new class for this widget.
		 *
		 * @param Class		The new class
		 */
		virtual void SetClass(const Ry::String& Class);

		/**
		 * Sets the style for this widget. This function should be overriden if the implementing widget has a concept of children widgets.
		 *
		 * @param Style		The style for this widget.
		 */
		virtual void SetStyle(const Ry::StyleSet* Style);

		/**
		 * Sets the parent of this widget. This function should only be called in very specific circumstances, usually when there is a child widget being added to this widget.
		 * The implementation of SetParent() in this class recalculates the depth, which is used for properly layering widgets.
		 *
		 * If the parent chain is changed, it is necessary to call this function again to correctly update the depth.
		 *
		 * @param Parent		The new parent widget.
		 */
		virtual void SetParent(Widget* Parent);

		/**
		 * Gets the pipeline state for a specified child widget. This allows the implementing widget to have separate dynamic pipeline states for different children.
		 *
		 * For example, in the case of a splitter, the scissor would be different for each child widget.
		 *
		 * @param ForWidget		The widget to get the pipeline state for. This widget MUST already be a child widget.
		 */
		virtual PipelineState GetPipelineState(const Widget* ForWidget) const;

		/**
		 * Gets the clip space for a particular widget.
		 * The default implementation ignores ForWidget and calls the parent widget's get clip space function. If there is no parent, the clip space is assumed to be the entire screen.
		 *
		 * @param ForWidget		The widget to get the clip space for.
		 * @return The clip space for the specified widget.
		 */
		virtual RectScissor GetClipSpace(const Widget* ForWidget) const;

		/**
		 * Correctly places this widget's geometry to match the absolute location and size. If the child has any custom geometry, this function needs to be overriden.
		 */
		virtual void Draw() {}

		/**
		 * Per tick logic for the widget. Allows the widget to perform animations.
		 */
		virtual void Update() {}

		/**
		 * Arranges the children widgets. A precondition to this function is that this own widget has already been positioned.
		 */
		virtual void Arrange();

		/**
		 * Called when the mouse hovers over this widget.
		 *
		 * @param MouseEv	A reference to the event.
		 */
		virtual void OnHovered(const MouseEvent& MouseEv);

		/**
		 * Called when the mouse leaves this widget.
		 *
		 * @param MouseEv	A reference to the event.
		 */
		virtual void OnUnhovered(const MouseEvent& MouseEv);

		/**
		 * Called when the mouse left clicks on this widget.
		 *
		 * @param MouseEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnPressed(const MouseButtonEvent& MouseEv);

		/**
		 * Called when the mouse released a left click on this widget.
		 *
		 * @param MouseEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnReleased(const MouseButtonEvent& MouseEv);

		/**
		 * Called when a mouse position event happens.
		 *
		 * @param MouseEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnMouseEvent(const MouseEvent& MouseEv);

		/**
		 * Called when a mouse button event occured.
		 *
		 * @param MouseEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv);

		/**
		 * Called when the mouse has clicked.
		 *
		 * @param MouseEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnMouseClicked(const MouseClickEvent& MouseEv);

		/**
		 * Called when the mouse has dragged a button.
		 *
		 * @param MouseEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnMouseDragged(const MouseDragEvent& MouseEv);

		/**
		 * Called when the mouse has scrolled.
		 *
		 * @param MouseEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnMouseScroll(const MouseScrollEvent& MouseEv);

		/**
		 * Called when a key has been pressed.
		 *
		 * @param KeyEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnKey(const KeyEvent& KeyEv);

		/**
		 * Called when a character has been entered.
		 *
		 * @param CharEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnChar(const CharEvent& CharEv);

		/**
		 * Called when operating system paths have been dropped into the window.
		 *
		 * @param PathDropEv	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnPathDrop(const PathDropEvent& PathDropEv);

		/**
		 * Called when an event has happened. The default implementation of this function simply forwards the event to a more specific event function in this class.
		 *
		 * @param Ev	A reference to the event.
		 * @return Whether this widget has handled the event.
		 */
		virtual bool OnEvent(const Event& Ev);

		/**
		 * Gathers all pipeline states, with the option to recurse into the pipeline states of the children.
		 *
		 * @param OutStates [out]	The destination of the found states.
		 * @param bRecurse [in]		Whether to recurse into children widgets.
		 */
		virtual void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates, bool bRecurse = true);

		/**
		 * Calculates the maximum space available for this widget to occupy that has been scaled to fit everything into the window.
		 *
		 * @param ForWidget		The widget to get the scaled slot size for.
		 * @return The scaled slot size.
		 */
		virtual SizeType GetScaledSlotSize(const Widget* ForWidget) const;

		/**
		 * Calculates the maximum size a widget would occupy if we had infinite screen space. In reality, screen space is limited and therefore this value usually must be adjusted. Therefore, this function should not be directly used for sizing purposes. Instead, use the GetScaledSlotSize() function.
		 * This function is mostly used internally to ultimately determine what the scaled size will be.
		 *
		 * @param ForWidget		The widget to get the scaled slot size for.
		 * @return The 
		 */
		virtual SizeType GetUnscaledSlotSize(const Widget* ForWidget) const;

		/**
		 * Calculates how much size in total the widget is requesting to occupy, plus padding.
		 *
		 * This function is essentially a utility function that invokes GetUnscaledSlotSize() and adds the padding.
		 *
		 * @param ForWidget		The widget to check for
		 * @return The unscaled slot size, plus any padding.
		 */
		virtual SizeType GetUnscaledOccupiedSize(const Widget* ForWidget) const;

		/**
		 * Similar to GetUnscaledOccupiedSize(), this function will call GetScaledSlotSize() and add the padding.
		 *
		 * @param ForWidget		The widget to check for
		 * @return The total size available to a particular widget, plus padding.
		 */
		virtual SizeType GetScaledOccupiedSize(const Widget* ForWidget) const;

		/**
		 * Computes the physical size this slot occupies.
		 *
		 * If the widget desires the max possible size available to it, an implementation could simply return Widget::GetScaledSlotSize(this).
		 * Otherwise, the widget needs to return the physical space it occupies for layout purposes.
		 *
		 * @return The physical size of the widget.
		 */
		virtual SizeType ComputeSize() const { return SizeType{}; }

		/**
		 * Called when the widget should add its geometry to a particular batch.
		 *
		 * @param Batch		The batch to add the widget's geometry to.
		 */
		virtual void OnShow(Ry::Batch* Batch) {}

		/**
		 * Called when the widget should remove its geometry from a particular batch.
		 *
		 * @param Batch		The batch to remove the widget's geometry from.
		 */
		virtual void OnHide(Ry::Batch* Batch) {}

		/**
		 * Called by child widgets when a full refresh of the widget hierarchy is necessary. This is required when widgets change size or position and may affect the positioning of other widgets.
		 */
		virtual void FullRefresh();

		/**
		 * Called by a child widget that needs to have its geometry re-positioned.
		 */
		virtual void Rearrange(Widget* Widget = nullptr);

		/**
		 * Called by a child widget when the batch needs to be updated.
		 *
		 * Cases where this may be necessary include when the child's geometry or visibility changes.
		 */
		virtual void UpdateBatch();

	protected:

		Widget* FindTopParent();
		int32 GetWidgetID() const;

		const StyleSet* Style;
		SizeType CachedSize;
		Widget* Parent{};
		int32 WidgetLayer;
		Point RelativePosition;
		bool bHovered;
		bool bPressed;
		bool bVisible;

	private:

		Ry::Batch* Bat;
		bool bHasBeenShown = false;
		uint32 WidgetID;

	} RefClass();

}

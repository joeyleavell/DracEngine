#pragma once

#include "Core/Core.h"
#include "Core/Delegate.h"
#include "UIGen.h"

#define NewWidgetAssign(AssignTo, Type) (*( (AssignTo) = new (Type){}))
#define NewWidget(Type) (*(new (Type)))

namespace Ry
{

	class Batch;
	
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
		MulticastDelegate<> SizeDirty;

		Widget() :
		RelativePosition{ 0, 0 },
		MaxSize{0, 0},
		Parent(nullptr),
		ShapeBatch(nullptr),
		TextBatch(nullptr)
		{};
		
		virtual ~Widget() = default;

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

		void SetParent(Widget* Parent)
		{
			this->Parent = Parent;
		}

		virtual Widget& operator[](Ry::Widget& Child)
		{
			return *this;
		}

		virtual void SetShapeBatch(Batch* Shape)
		{
			this->ShapeBatch = Shape;
		}

		virtual void SetTextBatch(Batch* Text)
		{
			this->TextBatch = Text;
		}

		virtual void Arrange()
		{

		}

		virtual void Show() = 0;
		virtual void Hide() = 0;
		virtual void Draw() = 0;
		virtual SizeType ComputeSize() const = 0;

	protected:
		
		SizeType CachedSize;
		Widget* Parent;
		SizeType MaxSize;

		Batch* ShapeBatch;
		Batch* TextBatch;

	private:
		Point RelativePosition;

	};

}

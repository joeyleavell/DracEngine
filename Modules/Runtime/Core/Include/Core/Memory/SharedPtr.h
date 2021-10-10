#pragma once

#include "Core/Assert.h"
#include "Core/Platform.h"

namespace Ry
{

	class EXPORT_ONLY RefCounter
	{

	public:

		RefCounter()
		{
			this->Count = 1;
		}

		void Increment() const
		{
			this->Count++;
		}

		void Decrement() const
		{
			this->Count--;
		}

		int Value() const
		{
			return Count;
		}

		void SetValue(int Value) const
		{
			this->Count = Value;
		}

	private:

		mutable int Count;
	};

	template<class Class>
	class EXPORT_ONLY SharedPtr
	{

	public:

		SharedPtr():
		Value(nullptr),
		Counter(nullptr)
		{
		}

		/**
		 * Creates a new reference counter pointer to an object.
		 */
		/*SharedPtr(Class* Object):
		Value(nullptr),
		Counter(nullptr)
		{
			if(Object)
			{
				this->Value = Object;
				this->Counter = new RefCounter;
			}
		}*/

		SharedPtr(Class* Object, RefCounter* Counter):
		Value(nullptr),
		Counter(nullptr)
		{
			if(Object && Counter)
			{
				this->Value = Object;
				this->Counter = Counter;
			}
		}

		SharedPtr(const SharedPtr<Class>& Other) :
		Value(nullptr),
		Counter(nullptr)
		{
			Copy(Other);
		}

		template<class OtherClass>
		SharedPtr(const SharedPtr<OtherClass>& Other):
		Value(nullptr),
		Counter(nullptr)
		{
			Copy(Other);
		}

		SharedPtr(SharedPtr<Class>&& Other) noexcept:
		Value(nullptr),
		Counter(nullptr)
		{
			Move<Class>(std::move(Other));
		}

		template<class OtherClass>
		SharedPtr(SharedPtr<OtherClass>&& Other) noexcept :
		Value(nullptr),
		Counter(nullptr)
		{
			Move(std::move(Other));
		}


		~SharedPtr()
		{
			if(IsValid())
			{
				Counter->Decrement();

				if (Counter->Value() <= 0)
				{
					delete Value;
					delete Counter;
				}
			}

			// These two statements are VERY important for placement new construction
			Value = nullptr;
			Counter = nullptr;
		}

		/**
		 * Checks whether the pointer stored in this shared pointer is valid
		 */
		bool IsValid() const
		{
			return Value != nullptr;
		}

		/**
		 * Retrieves the underlying pointer
		 */
		Class* Get() const
		{
			return Value;
		}

		RefCounter* GetCounter() const
		{
			return Counter;
		}

		/**
		 * Dereferences the underlying pointer
		 */
		Class* operator*()
		{
			return Value;
		}

		/**
		 * Dereferences the underlying pointer
		 */
		Class* operator->() const
		{
			return Value;
		}

		template<class Parent>
		operator auto() const
		{
			Parent* CastResult = dynamic_cast<Parent*>(Value);

			if (CastResult)
			{
				// Copy to value and counter
				SharedPtr<Parent> Result(CastResult, Counter);

				Counter->Increment();
				
				return Result;
			}

			return SharedPtr<Parent>();
		}

		operator bool() const
		{
			return IsValid();
		}

		template<class OtherClass>
		bool operator==(const SharedPtr<OtherClass>& Other)
		{
			return Other.Get() == Get();
		}

		SharedPtr<Class>& operator=(SharedPtr<Class>&& Other) noexcept
		{
			Move(std::move(Other));

			return *this;
		}
		
		template<class OtherClass>
		SharedPtr<Class>& operator=(SharedPtr<OtherClass>&& Other) noexcept
		{
			Move(std::move(Other));

			return *this;
		}

		SharedPtr<Class>& operator=(const SharedPtr<Class>& Other)
		{
			Copy(Other);
			
			return *this;
		}

		template<class OtherClass>
		SharedPtr<Class>& operator=(const SharedPtr<OtherClass>& Other)
		{
			return Copy(Other);
		}

		// WARNING: for internal use only
		void Clear()
		{
			this->Counter = nullptr;
			this->Value = nullptr;
		}

		void Reset(Class* New)
		{
			// Relinquish share of existing pointer
			if (IsValid())
			{
				Counter->Decrement();

				if (Counter->Value() <= 0)
				{
					delete Value;
					delete Counter;
					this->Value = nullptr;
					this->Counter = nullptr;
				}
			}

			// Copy new pointer over
			if (New)
			{
				this->Counter = new RefCounter;
				this->Value = New;
			}
			else
			{
				this->Counter = nullptr;
				this->Value = nullptr;
			}
		}

	protected:

		Class* Value;
		RefCounter* Counter;

		template<class OtherClass>
		void Copy(const SharedPtr<OtherClass>& Other)
		{
			OtherClass* CastResult = dynamic_cast<OtherClass*>(Other.Get());

			CORE_ASSERT(!Other.IsValid() || CastResult);

			//CORE_ASSERT(CastResult);
			//CORE_ASSERT(Other.IsValid());

			// Relinquish share of existing pointer
			Reset(nullptr);

			// Copy new pointer over
			if (CastResult)
			{
				Other.GetCounter()->Increment();

				this->Counter = Other.GetCounter();
				this->Value = CastResult;
			}
			
		}

		template<class OtherClass>
		void Move(SharedPtr<OtherClass>&& Other)
		{
			OtherClass* CastResult = dynamic_cast<OtherClass*>(Other.Get());

			CORE_ASSERT(!Other.IsValid() || CastResult);

			// Relinquish share of existing pointer
			Reset(nullptr);
			
			if (CastResult)
			{
				this->Counter = Other.GetCounter();
				this->Value = CastResult;

				Other.Clear();
			}
		}

	};

	template<class To, class From>
	Ry::SharedPtr<To> CastShared(const Ry::SharedPtr<From>& Other)
	{
		To* CastResult = dynamic_cast<To*>(Other.Get());

		if(CastResult)
		{
			SharedPtr<To> Result (CastResult, Other.GetCounter());
			Result.GetCounter()->Increment();
			
			return Result;
		}

		return SharedPtr<To>();
	}

	template<class Class>
	Ry::SharedPtr<Class> MakeShared(Class* ToOwn)
	{
		Ry::SharedPtr<Class> RetPtr;
		RetPtr.Reset(ToOwn);
		return RetPtr;
	}
	
}

#pragma once

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
		SharedPtr(Class* Object):
		Value(nullptr),
		Counter(nullptr)
		{
			if(Object)
			{
				this->Value = Object;
				this->Counter = new RefCounter;
			}
		}

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

		//template<class OtherClass>
		SharedPtr(const SharedPtr<Class>& Other):
		Value(nullptr),
		Counter(nullptr)
		{			
			if(Other.IsValid())
			{
				//Class* Result = dynamic_cast<Class*>(Other.Get());

				//if(Result)
				{
					Other.GetCounter()->Increment();

					this->Counter = Other.GetCounter();
					this->Value = Other.Get();
//					this->Value = Result;

				}
			}
		}

		SharedPtr(SharedPtr<Class>&& Other) noexcept:
		Value(nullptr),
		Counter(nullptr)
		{
			if (Other.IsValid())
			{
				this->Counter = Other.GetCounter();
				this->Value = Other.Get();

				Other.Counter = nullptr;
				Other.Value = nullptr;
			}
		}

		~SharedPtr()
		{
			if(IsValid())
			{
				Counter->Decrement();

				if (Counter->Value() <= 0)
				{
					delete Value;
				}
			}
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
		Class* operator->()
		{
			return Value;
		}

		template<class Parent>
		operator SharedPtr<Parent>()
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

		SharedPtr<Class>& operator=(SharedPtr<Class>&& Other) noexcept
		{
			if (Other.IsValid())
			{
				this->Counter = Other.GetCounter();
				this->Value = Other.Get();

				Other.Counter = nullptr;
				Other.Value = nullptr;
			}

			return *this;
		}

		template<class OtherClass>
		bool operator==(const SharedPtr<OtherClass>& Other)
		{
			return Other.Get() == Get();
		}

		SharedPtr<Class>& operator=(const SharedPtr<Class>& Other)
		{
			if (&Other == this)
				return *this;
			
			// Relinquish share of existing pointer
			if(IsValid())
			{
				Counter->Decrement();

				if (Counter->Value() <= 0)
				{
					delete Value;
					this->Value = nullptr;
					this->Counter = nullptr;
				}
			}

			// Copy new pointer over
			if (Other.IsValid())
			{
				Other.Counter->Increment();

				this->Counter = Other.Counter;
				this->Value = Other.Value;
			}
			else
			{
				this->Counter = nullptr;
				this->Value = nullptr;
			}

			return *this;
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
		return Ry::SharedPtr<Class>(ToOwn);
		// return std::make_shared<T>(Arguments...);
	}
	
}

#pragma once

#include "Core/Core.h"
#include "Data/ArrayList.h"
#include <vector>

namespace Ry
{

	/**
	 * Represents the concept of an entity being "callable".
	 */
	template<typename Ret, typename ...Parameters>
	class EXPORT_ONLY Callable
	{
	public:
		virtual ~Callable() = default;

		virtual Callable<Ret, Parameters...>* Copy() = 0;
		
		virtual Ret Call(Parameters... Params) const = 0;
	};

	/**
	 * A function pointer callable.
	 */
	template <typename Ret, typename ...Parameters>
	class EXPORT_ONLY FunctionPtrCallable : public Callable<Ret, Parameters...>
	{
	public:

		FunctionPtrCallable(Ret (*Ptr)(Parameters...))
		{
			this->Data = Ptr;
		}

		Callable<Ret, Parameters...>* Copy() override
		{
			return new FunctionPtrCallable<Ret, Parameters...>(Data);
		}

		Ret Call(Parameters... Params) const override
		{
			return Data(Params...);
		}

		bool operator==(Ret(*Ptr)(Parameters...))
		{
			return Data == Ptr;
		}

		bool IsEqual(Ret (*SomePtr)(Parameters...))
		{
			return Data == SomePtr;
		}

	private:
		Ret (*Data)(Parameters...);
	};

	template<typename Ret, typename ...Parameters>
	class EXPORT_ONLY LambdaCallable : public Callable<Ret, Parameters...>
	{
	public:

		template <typename Function>
		LambdaCallable(Function&& Lambda)
		{
			Model<Function>* Callable = new Model<Function>(Lambda);

			// Defining this lambda so that we can use it as a function pointer
			Callable->NoCaptureLambda = [](Function Context, Parameters... Params) -> Ret
			{
				// Directly invoke the function here
				return Context(Params...);
			};

			InnerCallable = Callable;
		}

		Callable<Ret, Parameters...>* Copy() override
		{			
			return new LambdaCallable<Ret, Parameters...>(InnerCallable->Copy());
		}

		virtual ~LambdaCallable()
		{
			delete InnerCallable;
		}

		Ret Call(Parameters... Params) const override
		{
			return InnerCallable->Call(Params...);
		}

		template<typename Function>
		bool IsEqual(Function&& Ref)
		{
			Model<Function>* Downcasted = dynamic_cast<Model<Function>*>(InnerCallable);
			return Downcasted->Behavior == Ref;
		}

	private:

		LambdaCallable(Callable<Ret, Parameters...>* SomeCallable)
		{
			InnerCallable = SomeCallable;
		}

		/**
		 * Model for the type erasure.
		 */
		template<typename Function>
		struct Model final : public Callable<Ret, Parameters...>
		{
			const Function&& Behavior;
			Ret(*NoCaptureLambda)(Function Context, Parameters...);

			Model(const Function&& Func):
			Behavior(Func),
			NoCaptureLambda(nullptr)
			{
				
			}

			Model(const Function&& Func, Ret(*NoCapture)(Function Context, Parameters...)):
			Behavior(Func),
			NoCaptureLambda(NoCapture)
			{

			}

			virtual ~Model() = default;

			Callable<Ret, Parameters...>* Copy() override
			{
				return new Model<Function>(Behavior, NoCaptureLambda);
			}

			Ret Call(Parameters... Params) const override
			{	
				return NoCaptureLambda(Behavior, Params...);
			}
		};

		Callable<Ret, Parameters...>* InnerCallable;
	};


	/**
	 * A type-erased pointer-to-member callable.
	 */
	template<typename Ret, typename ...Parameters>
	class EXPORT_ONLY PointerToMemberCallable : public Callable<Ret, Parameters...>
	{
	public:

		template <class T>
		PointerToMemberCallable(T* ObjectPtr, Ret (T::*SomePtr)(Parameters...))
		{
			Model<T>* Callable = new Model<T>();
			Callable->Representation = ObjectPtr;
			Callable->Behavior = SomePtr;
			InnerCallable = Callable;
		}

		virtual ~PointerToMemberCallable()
		{
			delete InnerCallable;
		}

		Callable<Ret, Parameters...>* Copy() override
		{
			return new PointerToMemberCallable<Ret, Parameters...>(InnerCallable->Copy());
		}

		Ret Call(Parameters... Params) const override
		{
			return InnerCallable->Call(Params...);
		}

		template <class T>
		bool IsEqual(T* ObjectPtr, Ret(T::* SomePtr)(Parameters...))
		{
			Model<T>* Downcasted = dynamic_cast<Model<T>*>(InnerCallable);
			return ObjectPtr == Downcasted->Representation && SomePtr == Downcasted->Behavior;
		}
		
	private:

		PointerToMemberCallable(Callable<Ret, Parameters...>* SomeCallable)
		{
			InnerCallable = SomeCallable;
		}

		/**
		 * Model for the type erasure.
		 */
		template<class T>
		struct Model final : public Callable<Ret, Parameters...>
		{
			T* Representation;
			Ret (T::* Behavior)(Parameters...);

			Model():
			Representation(nullptr),
			Behavior(nullptr)
			{
			}

			virtual ~Model()
			{
				 
			}

			Callable<Ret, Parameters...>* Copy() override
			{
				auto NewModel = new Model<T>();
				NewModel->Representation = Representation;
				NewModel->Behavior = Behavior;

				return NewModel;
			}

			Ret Call(Parameters... Params) const override
			{
				return (Representation->*Behavior)(Params...);
			}
		};

		Callable<Ret, Parameters...>* InnerCallable;
	};

	template<typename Ret, typename ...Parameters>
	class EXPORT_ONLY Delegate
	{
	public:

		Delegate():
		InnerCallable(nullptr)
		{
		}

		Delegate(const Delegate<Ret, Parameters...>&& OtherDelegate) noexcept
		{
			// Copy the other's inner callable in case it gets deleted
			InnerCallable = OtherDelegate.InnerCallable->Copy();
		}

		~Delegate()
		{
			delete InnerCallable;
		}

		template<typename Function>
		static Delegate<Ret, Parameters...> CreateLambdaDelegate(Function&& Lambda)
		{
			Delegate<Ret, Parameters...> Result;
			Result.BindLambda(Lambda);
		
			return Result;
		}
		
		static Delegate<Ret, Parameters...> CreateFunctionDelegate(Ret (*SomeFunction) (Parameters...))
		{
			Delegate<Ret, Parameters...> Result;
			Result.BindFunction(SomeFunction);
		
			return Result;
		}
		
		template<typename T>
		static Delegate<Ret, Parameters...> CreateMemberFunctionDelegate(T* Instance, Ret(T::* SomeMemberFunctionPtr)(Parameters...))
		{
			Delegate<Ret, Parameters...> Result;
			Result.BindMemberFunction(Instance, SomeMemberFunctionPtr);
		
			return Result;
		}

		void BindFunction(Ret (*Ptr)(Parameters...))
		{
			delete InnerCallable;
			InnerCallable = nullptr;

			this->InnerCallable = new FunctionPtrCallable<Ret, Parameters...>(Ptr);
		}

		template<typename T>
		void BindMemberFunction(T* Instance, Ret (T::*SomeMemberFunctionPtr)(Parameters...))
		{
			delete InnerCallable;
			InnerCallable = nullptr;

			this->InnerCallable = new PointerToMemberCallable<Ret, Parameters...>(Instance, SomeMemberFunctionPtr);
		}

		template <typename Function>
		void BindLambda(Function&& Lambda)
		{
			delete InnerCallable;
			InnerCallable = nullptr;

			this->InnerCallable = new LambdaCallable<Ret, Parameters...>(Lambda);
		}

		bool IsBound()
		{
			return InnerCallable != nullptr;
		}

		Ret Execute(Parameters... Params) const
		{
			return InnerCallable->Call(Params...);
		}

		Delegate<Ret, Parameters...>& operator=(const Delegate<Ret, Parameters...>& Other)
		{
			InnerCallable = Other.InnerCallable->Copy();
			return *this;
		}
		
	private:

		Callable<Ret, Parameters...>* InnerCallable;
	};

	template<typename ...Parameters>
	class EXPORT_ONLY MulticastDelegate
	{
	public:

		MulticastDelegate() = default;
		
		~MulticastDelegate()
		{
			
		}

		void AddFunction(void(*Ptr)(Parameters...))
		{
			Listeners.Add(new FunctionPtrCallable<void, Parameters...>(Ptr));
		}

		template<class T>
		void AddMemberFunction(T* Instance, void (T::* SomeMemberFunctionPtr)(Parameters...))
		{
			Listeners.Add(new PointerToMemberCallable<void, Parameters...>(Instance, SomeMemberFunctionPtr));
		}

		template <typename Function>
		void AddLambda(Function&& Lambda)
		{
			Listeners.Add(new LambdaCallable<void, Parameters...>(Lambda));
		}

		void RemoveFunction(void(*Ptr)(Parameters...))
		{
			auto Equality = [Ptr](Callable<void, Parameters...>* Callable) -> bool
			{
				FunctionPtrCallable<void, Parameters...>* Downcasted = dynamic_cast<FunctionPtrCallable<void, Parameters...>>(Callable);

				if (Downcasted)
				{
					if (Downcasted->IsEqual(Ptr))
					{
						return true;
					}
				}

				return false;
			};

			RemoveCallable(Equality);
		}

		template<class T>
		void RemoveMemberFunction(T* Object, void (T::* Ptr)(Parameters...))
		{
			auto Equality = [Object, Ptr](Callable<void, Parameters...>* Callable) -> bool
			{
				PointerToMemberCallable<void, Parameters...>* Downcasted = dynamic_cast<PointerToMemberCallable<void, Parameters...>*>(Callable);

				if (Downcasted)
				{
					if (Downcasted->IsEqual(Object, Ptr))
					{
						return true;
					}
				}

				return false;
			};

			

			RemoveCallable(Equality);
		}

		template<typename Function>
		void RemoveLambda(Function&& Ref)
		{
			auto Equality = [Ref](Callable<void, Parameters...>* Callable) -> bool
			{
				LambdaCallable<void, Parameters...>* Downcasted = dynamic_cast<LambdaCallable<void, Parameters...>*>(Callable);

				if (Downcasted)
				{
					if (Downcasted->IsEqual(Ref))
					{
						return true;
					}
				}

				return false;
			};

			RemoveCallable(Equality);
		}

		bool IsBound() const
		{
			return Listeners.GetSize() > 0;
		}

		void Broadcast(Parameters... Params) const
		{
			for(int32 CallableIndex = 0; CallableIndex < Listeners.GetSize(); CallableIndex++)
			{
				Listeners[CallableIndex]->Call(Params...);
			}
		}

	private:

		template<typename Function>
		void RemoveCallable(Function&& Func)
		{
			int32 IndexToRemove = -1;

			for (int32 Index = 0; Index < Listeners.GetSize(); Index++)
			{
				Callable<void, Parameters...>* Listener = Listeners[Index];

				if(Func(Listener))
				{
					IndexToRemove = Index;
					break;
				}
			}

			if (IndexToRemove > 0)
			{
				Listeners.Remove(IndexToRemove);
				//Listeners.erase(Listeners.begin() + IndexToRemove);
			}
		}

		Ry::ArrayList<Callable<void, Parameters...>*> Listeners;
		
	};
	
}

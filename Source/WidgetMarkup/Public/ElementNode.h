// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Templates/Casts.h"
#include "Templates/SharedPointer.h"

#include <type_traits>

/** Lightweight type descriptor for FElementNode hierarchy. Used for IsA/Cast without RTTI. */
struct FElementNodeClass
{
	const FElementNodeClass* SuperClass;

	explicit FElementNodeClass(const FElementNodeClass* InSuperClass)
		: SuperClass(InSuperClass)
	{
	}

	bool IsChildOf(const FElementNodeClass* Other) const
	{
		for (const FElementNodeClass* Current = this; Current; Current = Current->SuperClass)
		{
			if (Current == Other)
			{
				return true;
			}
		}
		return false;
	}
};

class FElementNode : public TSharedFromThis<FElementNode>
{
	friend class FElementNodeFactory;
	friend class FElementTreeBuilder;

public:
	/** Returns this instance's type descriptor for IsA/Cast. */
	virtual const FElementNodeClass* GetClass() const = 0;

	static const FElementNodeClass* StaticClass();

	template <typename T>
	bool IsA() const
	{
		static_assert(std::is_base_of_v<FElementNode, T>, "T must be FElementNode or a derived class");
		return GetClass()->IsChildOf(T::StaticClass());
	}

	virtual UObject* GetObject() const { return nullptr; }

	enum class EMessageType
	{
		Log,
		Error,
	};

	class FMessage
	{
	public:
		FMessage(const FText& InText, EMessageType InType);
		const FText& GetText() const;

	private:
		FText Text;
		EMessageType Type;
	};

	class FResult
	{
	public:
		FResult& Log(const FText& MessageText);
		FResult& Error(const FText& MessageText);

		static FResult Success();
		static FResult Failure();

		operator bool() const;
		bool operator!() const;

		FResult& PrintOnFailure();

	private:
		FResult() = default;

		bool bOK = false;
		TArray<TSharedRef<FMessage>> Messages;
	};

protected:

	class FContext
	{
	public:
		void Push(const TSharedRef<FElementNode>& ElementNode);
		TSharedRef<FElementNode> Pop();
		bool IsEmpty() const;

		TSharedPtr<FElementNode> GetLastNode() const;

		/** Returns the first node (from top of stack) whose GetObject() is non-null. */
		TSharedPtr<FElementNode> GetLastObjectNode() const;

		const auto& GetNodes() const { return Nodes; }
		auto& GetNodes() { return Nodes; }

		UObject* FindObject(UClass* Class) const;

		template <typename T>
		T* FindObject() const
		{
			return Cast<T>(FindObject(T::StaticClass()));
		}

	private:
		TArray<TSharedRef<FElementNode>> Nodes;
	};

	FElementNode();
	virtual ~FElementNode();

	virtual void AddReferencedObjects(FReferenceCollector& Collector) {}
	virtual FResult Begin(const FContext& Context, UObject* Outer, UStruct* Struct) = 0;
	virtual FResult End() = 0;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) = 0;
	virtual bool HasProperty(const FStringView& AttributeName) = 0;
};

/** Injects StaticClass() and GetClass() for safe Cast/IsA. */
#define DECLARE_ELEMENT_NODE(TClass, TSuperClass) \
public: \
	using Super = TSuperClass; \
	static const FElementNodeClass* StaticClass(); \
	virtual const FElementNodeClass* GetClass() const override { return StaticClass(); }

/** Implements StaticClass() for a derived ElementNode. Place in the .cpp for TClass. */
#define IMPLEMENT_ELEMENT_NODE(TClass, TSuperClass) \
	const FElementNodeClass* TClass::StaticClass() \
	{ \
		static const FElementNodeClass Instance(TSuperClass::StaticClass()); \
		return &Instance; \
	}

template <typename T>
inline T* CastElementNode(FElementNode* Node)
{
	static_assert(std::is_base_of_v<FElementNode, T>, "T must be FElementNode or a derived class");
	return Node && Node->IsA<T>() ? static_cast<T*>(Node) : nullptr;
}

template <typename T>
inline const T* CastElementNode(const FElementNode* Node)
{
	static_assert(std::is_base_of_v<FElementNode, T>, "T must be FElementNode or a derived class");
	return Node && Node->IsA<T>() ? static_cast<const T*>(Node) : nullptr;
}

template <typename T>
inline TSharedPtr<T> CastElementNode(const TSharedPtr<FElementNode>& Node)
{
	static_assert(std::is_base_of_v<FElementNode, T>, "T must be FElementNode or a derived class");
	if (T* Ptr = CastElementNode<T>(Node.Get()))
	{
		return TSharedPtr<T>(Node, Ptr);
	}
	return TSharedPtr<T>();
}

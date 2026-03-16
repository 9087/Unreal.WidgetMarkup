// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "Templates/Casts.h"

class FElementNode : public TSharedFromThis<FElementNode>
{
	friend class FElementNodeFactory;
	friend class FElementTreeBuilder;

public:
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

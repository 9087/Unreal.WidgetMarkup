// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNodes/BufferedPropertyContext.h"
#include "ElementNodes/PropertyChainHandle.h"

#include "ElementNode.h"

class IPropertyRun;
class FPropertyChainHandle;
class FProperty;
struct FPropertyBuffer;

// ---- Container-element resolver traits (add Set/Map resolvers here as needed) ----

struct FArrayElementResolver
{
	static bool CanResolve(const FPropertyChainHandle& Chain) { return Chain.IsArrayProperty(); }
	static FWidgetPropertyPath AppendTo(const FWidgetPropertyPath& Base, int32 Index) { return Base.WithAppendedArrayIndex(Index); }
};

class FPropertyElementNode : public FElementNode, public FGCObject
{
	DECLARE_ELEMENT_NODE(FPropertyElementNode, FElementNode)

	friend class FElementTreeBuilder;
	friend class FPropertyRun;
	template<typename> friend bool TryResolveContainerElementPath(const FElementNode::FContext&, int32, FWidgetPropertyPath&, FBufferedPropertyContext&, FText*);

public:
	static TSharedRef<FElementNode> Create(const FStringView& InPropertyName, const FStringView& InPropertyValue);
	static bool TryResolvePropertyPath(
		const FContext& Context,
		const FStringView& PropertyName,
		bool& bInOutUseBufferedWrite,
		FWidgetPropertyPath& OutPropertyPath,
		FBufferedPropertyContext& OutBufferedPropertyContext,
		FText* OutError = nullptr);

	/** Resolve an array-element path (e.g. "ColumnFill[0]") from the parent context. */
	static bool TryResolveArrayElementPath(
		const FContext& Context,
		int32 ArrayIndex,
		FWidgetPropertyPath& OutPropertyPath,
		FBufferedPropertyContext& OutBufferedPropertyContext);

	/** Attribute form: (name, value). Element form: (element name, ElementData). */
	FPropertyElementNode(const FStringView& InPropertyName, const FStringView& InPropertyValue, bool bInUseBufferedWrite = false);

	virtual ~FPropertyElementNode() override;

	virtual UStruct* GetPropertyOwnerStruct() const override;

	FString GetPropertyPath() const { return PropertyPath.GetPathName().ToString(); }
	const FString& GetPropertyName() const { return PropertyName; }
	const FString& GetPropertyValue() const { return PropertyValue; }
	TSharedPtr<const FPropertyBuffer> GetPropertyBuffer() const { return BufferedPropertyContext.GetPropertyBuffer(); }
	int32 GetChildCount() const { return ElementChildren.Num(); }
	void SetPropertyRun(TSharedPtr<IPropertyRun> InPropertyRun);

protected:
	//~Begin FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	//~End FGCObject interface

	//~Begin FElementNode interface
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

	TSharedPtr<IPropertyRun> GetPropertyRunInternal() const { return PropertyRun; }

	/** Name/path passed at construction (segment or canonical property path). */
	FString PropertyName;
	/** Full canonical property path from nearest object; computed in Begin(). */
	FWidgetPropertyPath PropertyPath;
	/** Buffered write context shared by nested property nodes. */
	FBufferedPropertyContext BufferedPropertyContext;
	/** Value passed at construction (attribute value or ElementData). */
	FString PropertyValue;
	/** When true, writes go to temporary property memory and commit in End(). */
	bool bUseBufferedWrite = false;

	TSharedPtr<FPropertyChainHandle> PropertyChain;

	/** Child element nodes. SetValue in End() only when Num() == 0. */
	TArray<TSharedRef<FElementNode>> ElementChildren;

	TSharedPtr<IPropertyRun> PropertyRun;
};

template<typename TResolver>
bool TryResolveContainerElementPath(
	const FElementNode::FContext& Context,
	int32 Index,
	FWidgetPropertyPath& OutPropertyPath,
	FBufferedPropertyContext& OutBufferedPropertyContext,
	FText* OutError = nullptr)
{
	TSharedPtr<FElementNode> Parent = Context.GetLastNode();
	OutPropertyPath.Reset();
	OutBufferedPropertyContext.Reset();

	auto PropertyParent = CastElementNode<FPropertyElementNode>(Parent.Get());
	if (!PropertyParent || !PropertyParent->PropertyChain.IsValid() || !TResolver::CanResolve(*PropertyParent->PropertyChain))
	{
		if (OutError)
		{
			*OutError = FText::FromString(TEXT("Parent is not the expected container type."));
		}
		return false;
	}

	OutPropertyPath = TResolver::AppendTo(PropertyParent->PropertyPath, Index);
	OutBufferedPropertyContext = PropertyParent->BufferedPropertyContext;
	return true;
}

inline bool FPropertyElementNode::TryResolveArrayElementPath(
	const FContext& Context,
	int32 ArrayIndex,
	FWidgetPropertyPath& OutPropertyPath,
	FBufferedPropertyContext& OutBufferedPropertyContext)
{
	return TryResolveContainerElementPath<FArrayElementResolver>(Context, ArrayIndex, OutPropertyPath, OutBufferedPropertyContext);
}
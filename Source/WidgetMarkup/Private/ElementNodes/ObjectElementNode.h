// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNode.h"

class FObjectElementNode : public FElementNode, public FGCObject
{
	DECLARE_ELEMENT_NODE(FObjectElementNode, FElementNode)

	//~Begin FElementNode interface
public:
	virtual UObject* GetObject() const override;
	virtual UStruct* GetPropertyOwnerStruct() const override;

	/** Returns the asset path when this node represents a path reference (non-empty only in path-ref mode). */
	const FString& GetObjectPath() const { return ObjectPath; }

	/** True when this node is a path reference; false when it is an inline definition. */
	bool IsPathReference() const { return !ObjectPath.IsEmpty(); }

protected:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	virtual void SetElementData(const TCHAR* InElementData) override;
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

	TObjectPtr<UObject> Object = nullptr;

	/** Asset path set via element text content for path-reference mode (e.g. /Game/Data/Asset1). */
	FString ObjectPath;
};
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

protected:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

	TObjectPtr<UObject> Object = nullptr;
};
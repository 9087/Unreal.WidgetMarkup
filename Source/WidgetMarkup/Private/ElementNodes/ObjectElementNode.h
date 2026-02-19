// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNode.h"

class FObjectElementNode : public FElementNode
{
protected:
	//~Begin FElementNode interface
	virtual UObject* GetObject() const override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FResult Begin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult End() override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

	TObjectPtr<UObject> Object = nullptr;
};
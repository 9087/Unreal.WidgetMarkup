// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNode.h"

class FPropertyChainHandle;

class FPropertyElementNode : public FElementNode
{
public:
	FPropertyElementNode(const FStringView& InPropertyName, const FStringView& InPropertyValue);

protected:
	//~Begin FElementNode interface
	virtual FResult Begin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult End() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;
	//~End FElementNode interface

	TSharedPtr<FElementNode> Container;
	const FStringView& PropertyName;
	const FStringView& PropertyValue;

	TSharedPtr<FPropertyChainHandle> PropertyChain;
};

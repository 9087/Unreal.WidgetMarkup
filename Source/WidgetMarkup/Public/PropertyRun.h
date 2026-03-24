// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNode.h"

class IPropertyRun : public TSharedFromThis<IPropertyRun>
{
public:
	virtual ~IPropertyRun() = default;
	virtual FElementNode::FResult OnBegin(FElementNode::FContext& Context, UObject* Outer, const FStringView& PropertyName, const FStringView& PropertyValue) = 0;
	virtual FElementNode::FResult OnEnd(FElementNode::FContext& Context) = 0;
};

class FPropertyRun : public IPropertyRun
{
protected:
	virtual FElementNode::FResult OnBegin(FElementNode::FContext& Context, UObject* Outer, const FStringView& PropertyName, const FStringView& PropertyValue) override;
	virtual FElementNode::FResult OnEnd(FElementNode::FContext& Context) override;

	TWeakPtr<FElementNode> ElementNode;
};
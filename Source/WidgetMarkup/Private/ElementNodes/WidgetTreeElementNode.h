// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ObjectElementNode.h"

class FWidgetTreeElementNode : public FObjectElementNode
{
	DECLARE_ELEMENT_NODE(FWidgetTreeElementNode, FObjectElementNode)

public:
	static TSharedRef<FElementNode> Create();

protected:
	//~Begin FElementNode interface
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	//~End FElementNode interface
};
// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ObjectElementNode.h"

class FWidgetBlueprintElementNode : public FObjectElementNode
{
	DECLARE_ELEMENT_NODE(FWidgetBlueprintElementNode, FObjectElementNode)

public:
	static TSharedRef<FElementNode> Create();

protected:
	//~Begin FElementNode interface
	virtual FResult Begin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult End() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child);
	//~End FElementNode interface
};

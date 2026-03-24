// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "BlueprintElementNode.h"

class FWidgetBlueprintElementNode : public FBlueprintElementNode
{
	DECLARE_ELEMENT_NODE(FWidgetBlueprintElementNode, FBlueprintElementNode)

public:
	static TSharedRef<FElementNode> Create();

protected:
	//~Begin FElementNode interface
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child);
	//~End FElementNode interface

private:
	bool bHasWidgetTree = false;
};

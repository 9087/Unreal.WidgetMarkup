// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "StructElementNode.h"

class UBlueprint;

class FBlueprintVariableElementNode : public FStructElementNode
{
	DECLARE_ELEMENT_NODE(FBlueprintVariableElementNode, FStructElementNode)

public:
	static TSharedRef<FElementNode> Create();

protected:
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;

private:
	TWeakObjectPtr<UBlueprint> ParentBlueprint;
};

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ObjectElementNode.h"

class FBlueprintElementNode : public FObjectElementNode
{
	DECLARE_ELEMENT_NODE(FBlueprintElementNode, FObjectElementNode)

public:
	static TSharedRef<FElementNode> Create();

protected:
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;

	FResult CreateOrReuseBlueprint(UPackage* Package, UClass* ParentClass, UClass* BlueprintClass, UClass* GeneratedClass);
};

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "ElementNode.h"

class UBlueprint;

class FBlueprintVariableElementNode : public FElementNode
{
	DECLARE_ELEMENT_NODE(FBlueprintVariableElementNode, FElementNode)

public:
	static TSharedRef<FElementNode> Create();
	FResult ApplyVariableAttribute(const FStringView& InAttributeName, const FStringView& InAttributeValue);

protected:
	virtual FResult OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct) override;
	virtual FResult OnEnd() override;
	virtual FResult OnAddChild(const TSharedRef<FElementNode>& Child) override;
	virtual bool HasProperty(const FStringView& AttributeName) override;

private:
	TWeakObjectPtr<UBlueprint> ParentBlueprint;
	FString VariableName;
	FString VariableType;
	FString VariableDefaultValue;
	bool bNameSet = false;
	bool bTypeSet = false;
};

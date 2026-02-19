// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetElementNode.h"

#include "Blueprint/WidgetTree.h"

TSharedRef<FElementNode> FWidgetElementNode::Create()
{
	return MakeShared<FWidgetElementNode>();
}

FElementNode::FResult FWidgetElementNode::Begin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	if (auto WidgetTree = Context.FindObject<UWidgetTree>())
	{
		Outer = WidgetTree;
	}
	return FObjectElementNode::Begin(Context, Outer, Struct);
}

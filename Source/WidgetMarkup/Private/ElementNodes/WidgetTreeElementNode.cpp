// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetTreeElementNode.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"

IMPLEMENT_ELEMENT_NODE(FWidgetTreeElementNode, FObjectElementNode)

TSharedRef<FElementNode> FWidgetTreeElementNode::Create()
{
	return MakeShared<FWidgetTreeElementNode>();
}

FElementNode::FResult FWidgetTreeElementNode::Begin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	if (auto WidgetBlueprint = Context.FindObject<UWidgetBlueprint>())
	{
		Outer = WidgetBlueprint;
	}
	return FObjectElementNode::Begin(Context, Outer, Struct);
}

FElementNode::FResult FWidgetTreeElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	auto Widget = Cast<UWidget>(Child->GetObject());
	if (!Widget)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("WidgetTreeElementNode: child element must resolve to a UWidget instance.")));
	}
	auto WidgetTree = Cast<UWidgetTree>(Object);
	if (!WidgetTree)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Root widget of Widget Tree must be of type Widget.")));
	}
	WidgetTree->RootWidget = Widget;
	return FResult::Success();
}

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetBlueprintElementNode.h"

#include "BlueprintElementNode.h"
#include "Extensions/WidgetMarkupBlueprintExtension.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "WidgetBlueprintExtension.h"

IMPLEMENT_ELEMENT_NODE(FWidgetBlueprintElementNode, FBlueprintElementNode)

TSharedRef<FElementNode> FWidgetBlueprintElementNode::Create()
{
	return MakeShared<FWidgetBlueprintElementNode>();
}

FElementNode::FResult FWidgetBlueprintElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	auto Package = Cast<UPackage>(Outer);
	if (!Package)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: outer object must be a UPackage when creating a Widget Blueprint.")));
	}

	FResult Result = CreateOrReuseBlueprint(Package, UUserWidget::StaticClass(), UWidgetBlueprint::StaticClass(), UWidgetBlueprintGeneratedClass::StaticClass());
	if (!Result)
	{
		return Result;
	}

	UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Object);
	if (!WidgetBlueprint)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: failed to cast stored object to UWidgetBlueprint after creation.")));
	}

	UWidgetBlueprintExtension::RequestExtension<UWidgetMarkupBlueprintExtension>(WidgetBlueprint);
	return FResult::Success();
}

FElementNode::FResult FWidgetBlueprintElementNode::OnEnd()
{
	// Delegate to parent Blueprint compilation (same logic for compiling)
	return FBlueprintElementNode::OnEnd();
}

FElementNode::FResult FWidgetBlueprintElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	auto Blueprint = Cast<UWidgetBlueprint>(Object);
	if (!Blueprint)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: stored object is not a valid UWidgetBlueprint instance.")));
	}

	// Check for WidgetTree (WidgetBlueprint-specific)
	if (auto WidgetTree = Cast<UWidgetTree>(Child->GetObject()))
	{
		if (bHasWidgetTree)
		{
			return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: only one WidgetTree child is allowed.")));
		}
		Blueprint->WidgetTree = WidgetTree;
		bHasWidgetTree = true;
		return FResult::Success();
	}

	// Delegate to parent for Variable handling (generic blueprint variable support)
	return FBlueprintElementNode::OnAddChild(Child);
}

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetBlueprintElementNode.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet2/KismetEditorUtilities.h"

IMPLEMENT_ELEMENT_NODE(FWidgetBlueprintElementNode, FObjectElementNode)

TSharedRef<FElementNode> FWidgetBlueprintElementNode::Create()
{
	return MakeShared<FWidgetBlueprintElementNode>();
}

FElementNode::FResult FWidgetBlueprintElementNode::Begin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	auto Package = Cast<UPackage>(Outer);
	if (!Package)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: outer object must be a UPackage when creating a Widget Blueprint.")));
	}
	auto WidgetBlueprintName = FName(FPaths::GetBaseFilename(Package->GetName(), true));
	auto Blueprint = FindObject<UBlueprint>(Package, *WidgetBlueprintName.ToString());
	if (!Blueprint)
	{
		Blueprint = FKismetEditorUtilities::CreateBlueprint(
			UUserWidget::StaticClass(),
			Package,
			WidgetBlueprintName,
			BPTYPE_Normal,
			UWidgetBlueprint::StaticClass(),
			UWidgetBlueprintGeneratedClass::StaticClass(),
			NAME_None
		);
	}
	Object = Blueprint;
	return FResult::Success();
}

FElementNode::FResult FWidgetBlueprintElementNode::End()
{
	auto Blueprint = Cast<UWidgetBlueprint>(Object);
	if (!Blueprint)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: failed to cast stored object to UWidgetBlueprint before compile.")));
	}
	EBlueprintCompileOptions CompileOptions = EBlueprintCompileOptions::None;
	CompileOptions |= EBlueprintCompileOptions::IncludeCDOInReferenceReplacement;
	FKismetEditorUtilities::CompileBlueprint(Blueprint, CompileOptions);
	return FResult::Success();
}

FElementNode::FResult FWidgetBlueprintElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	auto Blueprint = Cast<UWidgetBlueprint>(Object);
	if (!Blueprint)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: stored object is not a valid UWidgetBlueprint instance.")));
	}
	if (auto WidgetTree = Cast<UWidgetTree>(Child->GetObject()))
	{
		Blueprint->WidgetTree = WidgetTree;
		return FResult::Success();
	}
	return FResult::Failure().Error(FText::FromString(TEXT("WidgetBlueprintElementNode: child element must resolve to a UWidgetTree instance.")));
}

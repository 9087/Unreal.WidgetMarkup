// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "WidgetBlueprintElementNode.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet2/KismetEditorUtilities.h"

TSharedRef<FElementNode> FWidgetBlueprintElementNode::Create()
{
	return MakeShared<FWidgetBlueprintElementNode>();
}

FElementNode::FResult FWidgetBlueprintElementNode::Begin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	auto Package = Cast<UPackage>(Outer);
	if (!Package)
	{
		return FResult::Failure();
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
		return FResult::Failure();
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
		return FResult::Failure();
	}
	if (auto WidgetTree = Cast<UWidgetTree>(Child->GetObject()))
	{
		Blueprint->WidgetTree = WidgetTree;
		return FResult::Success();
	}
	return FResult::Failure();
}

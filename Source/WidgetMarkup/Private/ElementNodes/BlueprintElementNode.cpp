// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "BlueprintElementNode.h"

#include "BlueprintVariableElementNode.h"
#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

IMPLEMENT_ELEMENT_NODE(FBlueprintElementNode, FObjectElementNode)

TSharedRef<FElementNode> FBlueprintElementNode::Create()
{
	return MakeShared<FBlueprintElementNode>();
}

FElementNode::FResult FBlueprintElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	UPackage* Package = Cast<UPackage>(Outer);
	if (!Package)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("BlueprintElementNode: outer object must be a UPackage when creating a Blueprint.")));
	}

	return CreateOrReuseBlueprint(Package, UObject::StaticClass(), UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass());
}

FElementNode::FResult FBlueprintElementNode::CreateOrReuseBlueprint(UPackage* Package, UClass* ParentClass, UClass* BlueprintClass, UClass* GeneratedClass)
{
	const FName BlueprintName(FPaths::GetBaseFilename(Package->GetName(), true));
	UBlueprint* Blueprint = FindObject<UBlueprint>(Package, *BlueprintName.ToString());
	if (!Blueprint)
	{
		Blueprint = FKismetEditorUtilities::CreateBlueprint(
			ParentClass,
			Package,
			BlueprintName,
			BPTYPE_Normal,
			BlueprintClass,
			GeneratedClass,
			NAME_None
		);
	}
	else
	{
		TArray<FName> VarNames;
		VarNames.Reserve(Blueprint->NewVariables.Num());
		for (const FBPVariableDescription& Var : Blueprint->NewVariables)
		{
			VarNames.Add(Var.VarName);
		}
		if (VarNames.Num() > 0)
		{
			FBlueprintEditorUtils::BulkRemoveMemberVariables(Blueprint, VarNames);
		}

		Blueprint->ImplementedInterfaces.Empty();
	}

	Object = Blueprint;
	return FResult::Success();
}

FElementNode::FResult FBlueprintElementNode::OnEnd()
{
	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	if (!Blueprint)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("BlueprintElementNode: failed to cast stored object to UBlueprint before compile.")));
	}

	EBlueprintCompileOptions CompileOptions = EBlueprintCompileOptions::None;
	CompileOptions |= EBlueprintCompileOptions::IncludeCDOInReferenceReplacement;
	FKismetEditorUtilities::CompileBlueprint(Blueprint, CompileOptions);
	return FResult::Success();
}

FElementNode::FResult FBlueprintElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	if (Child->IsA<FBlueprintVariableElementNode>())
	{
		return FResult::Success();
	}
	return FResult::Failure().Error(FText::FromString(TEXT("BlueprintElementNode: only Variable child elements are supported for Blueprint root node.")));
}

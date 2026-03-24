// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/BlueprintSuperPropertyRun.h"

#include "Engine/Blueprint.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Utilities/TypeParser.h"

TSharedRef<IPropertyRun> FBlueprintSuperPropertyRun::Create()
{
	return MakeShared<FBlueprintSuperPropertyRun>();
}

FElementNode::FResult FBlueprintSuperPropertyRun::OnBegin(FElementNode::FContext& /*Context*/, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	if (!PropertyName.Equals(TEXT("Super"), ESearchCase::IgnoreCase))
	{
		return FElementNode::FResult::Failure();
	}

	UBlueprint* Blueprint = Cast<UBlueprint>(Object);
	if (!Blueprint)
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Super property target is not a Blueprint.")));
	}

	const FString SuperText = FString(PropertyValue).TrimStartAndEnd();
	if (SuperText.IsEmpty())
	{
		Blueprint->ParentClass = UObject::StaticClass();
		return FElementNode::FResult::Success();
	}

	UClass* ResolvedClass = FTypeParser::ResolveClass(SuperText);
	if (!ResolvedClass)
	{
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Super property '{0}' cannot be resolved to a valid UClass.")),
			FText::FromString(SuperText)));
	}

	if (!FKismetEditorUtilities::CanCreateBlueprintOfClass(ResolvedClass))
	{
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Super class '{0}' cannot be used to create a Blueprint.")),
			FText::FromString(ResolvedClass->GetName())));
	}

	Blueprint->ParentClass = ResolvedClass;
	return FElementNode::FResult::Success();
}

FElementNode::FResult FBlueprintSuperPropertyRun::OnEnd(FElementNode::FContext& /*Context*/)
{
	return FElementNode::FResult::Success();
}

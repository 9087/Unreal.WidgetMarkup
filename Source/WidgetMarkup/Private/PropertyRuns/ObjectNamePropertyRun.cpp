// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/ObjectNamePropertyRun.h"

#include "Components/Widget.h"

namespace
{
struct FObjectNamePropertyMetaData : public FElementNode::FContext::TMetaData<FObjectNamePropertyMetaData>
{
	TMap<FString, FString> UsedNames;
};
}

TSharedRef<IPropertyRun> FObjectNamePropertyRun::Create()
{
	return MakeShared<FObjectNamePropertyRun>();
}

FElementNode::FResult FObjectNamePropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue)
{
	if (!PropertyName.Equals(TEXT("Name"), ESearchCase::IgnoreCase))
	{
		return FElementNode::FResult::Failure();
	}

	if (!Object)
	{
		return FElementNode::FResult::Failure().Error(FText::FromString(TEXT("Name property target object is null.")));
	}

	FString Name = FString(PropertyValue).TrimStartAndEnd();
	if (Name.IsEmpty())
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Name property on '{0}' must not be empty.")),
				FText::FromString(Object->GetClass()->GetName())));
	}

	if (Name.Contains(TEXT(".")) || Name.Contains(TEXT(" ")))
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Name property '{0}' contains invalid characters (no '.' or spaces allowed).")),
				FText::FromString(Name)));
	}

	TSharedRef<FObjectNamePropertyMetaData> MetaData = Context.GetOrAddMetaData<FObjectNamePropertyMetaData>();
	const FString NormalizedName = Name.ToLower();
	if (const FString* ExistingDescription = MetaData->UsedNames.Find(NormalizedName))
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Duplicate Name '{0}': already used by {1} in this blueprint.")),
				FText::FromString(Name),
				FText::FromString(*ExistingDescription)));
	}

	MetaData->UsedNames.Add(NormalizedName, FString::Printf(TEXT("%s (%s)"), *Object->GetClass()->GetName(), *Object->GetName()));

	if (!Object->Rename(*Name, nullptr, REN_DontCreateRedirectors | REN_NonTransactional))
	{
		return FElementNode::FResult::Failure().Error(
			FText::Format(
				FText::FromString(TEXT("Failed to rename object to '{0}' - name may already be in use in this scope.")),
				FText::FromString(Name)));
	}

	if (UWidget* Widget = Cast<UWidget>(Object))
	{
		Widget->bIsVariable = true;
	}

	return FElementNode::FResult::Success();
}

FElementNode::FResult FObjectNamePropertyRun::OnEnd(FElementNode::FContext& /*Context*/)
{
	return FElementNode::FResult::Success();
}

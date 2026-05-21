// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyRuns/ObjectNamePropertyRun.h"

#include "Binding/WidgetPropertyBindingCollection.h"
#include "Binding/WidgetPropertyBindingUtility.h"
#include "Components/Widget.h"

namespace
{
bool ValidateWidgetMarkupObjectName(UObject* Object, const FString& Name, FText& OutError)
{
	if (!Object)
	{
		OutError = FText::FromString(TEXT("Name property target object is null."));
		return false;
	}

	if (Name.IsEmpty())
	{
		OutError = FText::Format(
			FText::FromString(TEXT("Name property on '{0}' must not be empty.")),
			FText::FromString(Object->GetClass()->GetName()));
		return false;
	}

	if (Name.Contains(TEXT(".")) || Name.Contains(TEXT(" ")))
	{
		OutError = FText::Format(
			FText::FromString(TEXT("Name property '{0}' contains invalid characters (no '.' or spaces allowed).")),
			FText::FromString(Name));
		return false;
	}

	return true;
}
}

bool FObjectNamePropertyMetaData::TryApplyWidgetMarkupObjectName(FElementNode::FContext& Context, UObject* Object, const FString& Name, FText& OutError)
{
	if (!ValidateWidgetMarkupObjectName(Object, Name, OutError))
	{
		return false;
	}

	TSharedRef<FObjectNamePropertyMetaData> MetaData = Context.GetOrAddMetaData<FObjectNamePropertyMetaData>();
	if (const FString* ExistingDescription = MetaData->UsedNames.Find(Name))
	{
		OutError = FText::Format(
			FText::FromString(TEXT("Duplicate Name '{0}': already used by {1} in this blueprint.")),
			FText::FromString(Name),
			FText::FromString(*ExistingDescription));
		return false;
	}

	const FName PreviousName = Object->GetFName();
	if (!Object->Rename(*Name, nullptr, REN_DontCreateRedirectors | REN_NonTransactional))
	{
		OutError = FText::Format(
			FText::FromString(TEXT("Failed to rename object to '{0}' - name may already be in use in this scope.")),
			FText::FromString(Name));
		return false;
	}

	MetaData->UsedNames.Add(Name, FString::Printf(TEXT("%s (%s)"), *Object->GetClass()->GetName(), *Object->GetName()));

	if (TSharedPtr<FWidgetPropertyBindingCollection> BindingCollection = Context.GetMetaData<FWidgetPropertyBindingCollection>())
	{
		for (FWidgetPropertyBinding& Binding : BindingCollection->Bindings)
		{
			if (Binding.TargetObjectName == PreviousName)
			{
				Binding.TargetObjectName = Object->GetFName();
			}
		}
	}

	if (UWidget* Widget = Cast<UWidget>(Object))
	{
		Widget->bIsVariable = true;
	}

	return true;
}

bool FObjectNamePropertyMetaData::TryApplyGeneratedWidgetMarkupObjectName(FElementNode::FContext& Context, UObject* Object, FText& OutError)
{
	for (int32 Index = 0; Index < 10000; ++Index)
	{
		const FString Name = FString::Printf(TEXT("__WidgetMarkupBindingTarget_%d"), Index);
		if (TryApplyWidgetMarkupObjectName(Context, Object, Name, OutError))
		{
			return true;
		}
	}

	OutError = FText::FromString(TEXT("Failed to generate a unique WidgetMarkup binding target name."));
	return false;
}

bool FObjectNamePropertyMetaData::IsWidgetMarkupObjectNameRegistered(FElementNode::FContext& Context, UObject* Object)
{
	if (!Object)
	{
		return false;
	}

	TSharedPtr<FObjectNamePropertyMetaData> MetaData = Context.GetMetaData<FObjectNamePropertyMetaData>();
	return MetaData.IsValid() && MetaData->UsedNames.Contains(Object->GetName());
}

TSharedRef<IPropertyRun> FObjectNamePropertyRun::Create()
{
	return MakeShared<FObjectNamePropertyRun>();
}

FElementNode::FResult FObjectNamePropertyRun::OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& /*PropertyName*/, const FStringView& PropertyValue)
{
	FWidgetPropertyBindingToken BindingToken;
	if (TryParseWidgetPropertyBindingToken(PropertyValue, BindingToken))
	{
		return FElementNode::FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Name does not support binding expression '{{{0}}}'.")),
			FText::FromString(BindingToken.SourceExpression)));
	}

	FString Name = UnescapeWidgetPropertyBindingLiteral(PropertyValue).TrimStartAndEnd();
	FText Error;
	if (!FObjectNamePropertyMetaData::TryApplyWidgetMarkupObjectName(Context, Object, Name, Error))
	{
		return FElementNode::FResult::Failure().Error(Error);
	}

	return FElementNode::FResult::Success();
}

FElementNode::FResult FObjectNamePropertyRun::OnEnd(FElementNode::FContext& /*Context*/)
{
	return FElementNode::FResult::Success();
}

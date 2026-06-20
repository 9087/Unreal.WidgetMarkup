// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertySetter.h"

#include "PropertyBuffer.h"
#include "UObject/UnrealType.h"
#include "WidgetMarkupModule.h"

bool FPropertySetter::SetValue(
	void* InContainer,
	const FWidgetPropertyPath& InPropertyPath,
	FProperty* InTargetProperty,
	void* InTargetValueAddress,
	const FPropertyBuffer& InPropertyBuffer) const
{
	if (!InContainer || InPropertyPath.IsEmpty() || !InTargetProperty || !InTargetValueAddress || !InPropertyBuffer.HasValue())
	{
		return false;
	}

	FProperty* BufferedProperty = InPropertyBuffer.GetProperty();
	if (!BufferedProperty)
	{
		return false;
	}

	// Allow struct inheritance: child structs (e.g. FDeprecateSlateVector2D ← FVector2f) can
	// be set via the parent converter, since CopyCompleteValue handles the base memory layout.
	if (!InTargetProperty->SameType(BufferedProperty))
	{
		const FStructProperty* TargetStruct = CastField<FStructProperty>(InTargetProperty);
		const FStructProperty* BufferedStruct = CastField<FStructProperty>(BufferedProperty);
		if (!TargetStruct || !BufferedStruct ||
			(!TargetStruct->Struct->IsChildOf(BufferedStruct->Struct) &&
			 !BufferedStruct->Struct->IsChildOf(TargetStruct->Struct)))
		{
			return false;
		}
	}

	// 1. Try native setter declared via UPROPERTY(Setter=...) metadata.
	if (InTargetProperty->HasSetter())
	{
		InTargetProperty->CallSetter(InContainer, InPropertyBuffer.GetValueData());
		return true;
	}

	// 2. Fallback: direct property copy.
	InTargetProperty->CopyCompleteValue(InTargetValueAddress, InPropertyBuffer.GetValueData());
	return true;
}

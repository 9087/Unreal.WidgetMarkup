// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertySetter.h"

#include "PropertyBuffer.h"
#include "UObject/UnrealType.h"

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
	if (!BufferedProperty || !InTargetProperty->SameType(BufferedProperty))
	{
		return false;
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

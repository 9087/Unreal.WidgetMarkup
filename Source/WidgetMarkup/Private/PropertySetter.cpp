// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertySetter.h"

#include "PropertyBuffer.h"
#include "UObject/UnrealType.h"

bool FPropertySetter::SetValue(
	UObject* InObject,
	const FWidgetPropertyPath& InPropertyPath,
	FProperty* InTargetProperty,
	void* InTargetValueAddress,
	const FPropertyBuffer& InPropertyBuffer) const
{
	if (!InObject || InPropertyPath.IsEmpty() || !InTargetProperty || !InTargetValueAddress || !InPropertyBuffer.HasValue())
	{
		return false;
	}

	FProperty* BufferedProperty = InPropertyBuffer.GetProperty();
	if (!BufferedProperty || !InTargetProperty->SameType(BufferedProperty))
	{
		return false;
	}

	InTargetProperty->CopyCompleteValue(InTargetValueAddress, InPropertyBuffer.GetValueData());
	return true;
}

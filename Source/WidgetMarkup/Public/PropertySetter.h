// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utilities/WidgetPropertyPath.h"

class FProperty;
struct FPropertyBuffer;

class WIDGETMARKUP_API FPropertySetter : public TSharedFromThis<FPropertySetter>
{
public:
	virtual ~FPropertySetter() = default;

	/**
	 * Applies a prepared property buffer to the resolved target property.
	 *
	 * @param InObject The root object that owns the property path.
	 * @param InPropertyPath Canonical property path used to resolve the target.
	 * @param InTargetProperty The resolved target property definition.
	 * @param InTargetValueAddress The resolved writable target value address.
	 * @param InPropertyBuffer The source value buffer to apply.
	 */
	virtual bool SetValue(
		UObject* InObject,
		const FWidgetPropertyPath& InPropertyPath,
		FProperty* InTargetProperty,
		void* InTargetValueAddress,
		const FPropertyBuffer& InPropertyBuffer) const;
};

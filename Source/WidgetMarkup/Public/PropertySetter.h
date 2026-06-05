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
	 * @param InContainer The container object/struct that owns the target property.
	 * @param InPropertyPath Canonical property path used to resolve the target.
	 * @param InTargetProperty The resolved target property definition.
	 * @param InTargetValueAddress The resolved writable target value address.
	 * @param InPropertyBuffer The source value buffer to apply.
	 */
	virtual bool SetValue(
		void* InContainer,
		const FWidgetPropertyPath& InPropertyPath,
		FProperty* InTargetProperty,
		void* InTargetValueAddress,
		const FPropertyBuffer& InPropertyBuffer) const;
};

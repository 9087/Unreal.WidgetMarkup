// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "PropertyRun.h"

/**
 * Custom PropertyRun for FWidgetMarkupBlueprintVariable::Default.
 *
 * Suppresses binding-expression detection so that Variable defaults
 * are always treated as compile-time literal values.  Even values
 * that happen to look like {binding} syntax are stored as-is.
 */
class FVariableDefaultPropertyRun : public FPropertyRun
{
public:
	static TSharedRef<IPropertyRun> Create();

protected:
	virtual FElementNode::FResult OnBegin(
		FElementNode::FContext& Context,
		UObject* Outer,
		const FStringView& PropertyName,
		const FStringView& PropertyValue) override;
};

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "PropertyRun.h"

/**
 * Captures the Inherit attribute on <StyleSheet Inherit="..."> and stores the
 * path directly on the FStyleSheetElementNode via the parse context.
 */
class FStyleSheetInheritPropertyRun : public IPropertyRun
{
public:
	static TSharedRef<IPropertyRun> Create();

	virtual FElementNode::FResult OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue) override;
	virtual FElementNode::FResult OnEnd(FElementNode::FContext& Context) override;
};

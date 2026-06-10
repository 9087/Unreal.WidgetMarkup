// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "PropertyRun.h"

#include "Misc/Optional.h"

/**
 * Handles the UWidget::Style attribute (e.g. <Button Style="Primary"/>).
 * Not to be confused with the slate struct FWidgetStyle (unrelated).
 */
class FWidgetStylePropertyRun : public IPropertyRun
{
public:
	static TSharedRef<IPropertyRun> Create();

	virtual FElementNode::FResult OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue) override;
	virtual FElementNode::FResult OnEnd(FElementNode::FContext& Context) override;

private:
	TOptional<FName> StyleName;
};

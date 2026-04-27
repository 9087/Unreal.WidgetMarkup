// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "PropertyRun.h"

#include "Misc/Optional.h"

class FWidgetBlueprintScriptPropertyRun : public IPropertyRun
{
public:
	static TSharedRef<IPropertyRun> Create();

	virtual FElementNode::FResult OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue) override;
	virtual FElementNode::FResult OnEnd(FElementNode::FContext& Context) override;

private:
	TOptional<FString> Script;
};

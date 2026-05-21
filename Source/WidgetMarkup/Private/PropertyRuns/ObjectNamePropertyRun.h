// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

#include "PropertyRun.h"

struct FObjectNamePropertyMetaData : public FElementNode::FContext::TMetaData<FObjectNamePropertyMetaData>
{
	TMap<FString, FString> UsedNames;

	static bool TryApplyWidgetMarkupObjectName(FElementNode::FContext& Context, UObject* Object, const FString& Name, FText& OutError);
	static bool TryApplyGeneratedWidgetMarkupObjectName(FElementNode::FContext& Context, UObject* Object, FText& OutError);
	static bool IsWidgetMarkupObjectNameRegistered(FElementNode::FContext& Context, UObject* Object);
};

class FObjectNamePropertyRun : public IPropertyRun
{
public:
	static TSharedRef<IPropertyRun> Create();

	virtual FElementNode::FResult OnBegin(FElementNode::FContext& Context, UObject* Object, const FStringView& PropertyName, const FStringView& PropertyValue) override;
	virtual FElementNode::FResult OnEnd(FElementNode::FContext& Context) override;
};

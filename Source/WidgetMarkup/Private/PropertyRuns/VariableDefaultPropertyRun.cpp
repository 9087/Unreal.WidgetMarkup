// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "VariableDefaultPropertyRun.h"

#include "Binding/WidgetPropertyBindingCollection.h"

TSharedRef<IPropertyRun> FVariableDefaultPropertyRun::Create()
{
	return MakeShared<FVariableDefaultPropertyRun>();
}

FElementNode::FResult FVariableDefaultPropertyRun::OnBegin(
	FElementNode::FContext& Context,
	UObject* Outer,
	const FStringView& PropertyName,
	const FStringView& PropertyValue)
{
	// Variable Default is a compile-time literal — NOT a binding target.
	// Suppress the FWidgetPropertyAttributeValueScope so that FPropertyRun
	// treats the value as a plain string, even if it contains {braces}.
	Context.RemoveMetaData<FWidgetPropertyAttributeValueScope>();
	return FPropertyRun::OnBegin(Context, Outer, PropertyName, PropertyValue);
}

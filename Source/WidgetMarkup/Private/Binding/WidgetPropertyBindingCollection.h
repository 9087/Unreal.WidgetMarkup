#pragma once

#include "Binding/WidgetPropertyBinding.h"
#include "ElementNode.h"

struct FWidgetPropertyBindingCollection : public FElementNode::FContext::TMetaData<FWidgetPropertyBindingCollection>
{
	TArray<FWidgetPropertyBinding> Bindings;
};

struct FWidgetPropertyAttributeValueScope : public FElementNode::FContext::TMetaData<FWidgetPropertyAttributeValueScope>
{
};
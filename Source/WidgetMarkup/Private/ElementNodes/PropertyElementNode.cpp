// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "PropertyElementNode.h"

#include "PropertyChainHandle.h"

FPropertyElementNode::FPropertyElementNode(const FStringView& InPropertyName, const FStringView& InPropertyValue)
	: PropertyName(InPropertyName)
	, PropertyValue(InPropertyValue)
{
}

FElementNode::FResult FPropertyElementNode::Begin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	check(!Context.IsEmpty());
	Container = Context.GetLastNode();
	check(Container);
	if (auto Object = Container->GetObject())
	{
		PropertyChain = FPropertyChainHandle::Create(Object, PropertyName);
		if (PropertyChain && PropertyChain->SetValue(PropertyValue))
		{
			return FResult::Success();
		}
	}
	return FResult::Failure().Error(FText::Format(FText::FromString(TEXT("Failed to recognize the property name '{0}'.")), FText::FromStringView(PropertyName)));
}

FElementNode::FResult FPropertyElementNode::End()
{
	PropertyChain = nullptr;
	return FResult::Success();
}

FElementNode::FResult FPropertyElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	return FResult::Failure().Error(FText::FromString(TEXT("PropertyElementNode: property elements do not support child elements.")));
}

bool FPropertyElementNode::HasProperty(const FStringView& AttributeName)
{
	return PropertyChain->GetChildHandle(AttributeName).IsValid();
}
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

	TSharedPtr<FElementNode> ObjectNode = Context.GetLastObjectNode();
	if (!ObjectNode.IsValid())
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("No object node in context for property '{0}'.")),
			FText::FromStringView(PropertyName)));
	}

	UObject* Object = ObjectNode->GetObject();
	if (!Object)
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Object is null for property '{0}'.")),
			FText::FromStringView(PropertyName)));
	}

	// Compute full property path from context: parent is last node; if parent is PropertyElementNode, path = parent path + "." + PropertyName.
	TSharedPtr<FElementNode> Parent = Context.GetLastNode();
	if (!Parent.IsValid() || Parent == ObjectNode)
	{
		PropertyPath = PropertyName;
	}
	else
	{
		FPropertyElementNode* PropertyParent = static_cast<FPropertyElementNode*>(Parent.Get());
		PropertyPath = PropertyParent ? (PropertyParent->GetPropertyPath() + TEXT(".") + PropertyName) : PropertyName;
	}

	PropertyChain = FPropertyChainHandle::Create(Object, PropertyPath);
	if (!PropertyChain.IsValid())
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Failed to recognize the property path '{0}'.")),
			FText::FromString(PropertyPath)));
	}

	return FResult::Success();
}

FElementNode::FResult FPropertyElementNode::End()
{
	// Set value when there are no child elements and we have a value (attribute form has no children; element form only when no nested elements).
	const bool bShouldSetValue = PropertyChain.IsValid() && !PropertyValue.IsEmpty() && ElementChildren.Num() == 0;
	if (bShouldSetValue)
	{
		if (!PropertyChain->SetValue(PropertyValue))
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Failed to set value for property path '{0}'.")),
				FText::FromString(PropertyPath)));
		}
	}
	PropertyChain = nullptr;
	return FResult::Success();
}

FElementNode::FResult FPropertyElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	ElementChildren.Add(Child);
	return FResult::Success();
}

bool FPropertyElementNode::HasProperty(const FStringView& AttributeName)
{
	return PropertyChain.IsValid() && PropertyChain->GetChildHandle(AttributeName).IsValid();
}

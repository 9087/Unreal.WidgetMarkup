// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ObjectElementNode.h"

#include "PropertyChainHandle.h"
#include "PropertyElementNode.h"

IMPLEMENT_ELEMENT_NODE(FObjectElementNode, FElementNode)

UObject* FObjectElementNode::GetObject() const
{
	return Object;
}

UStruct* FObjectElementNode::GetPropertyOwnerStruct() const
{
	return Object ? Object->GetClass() : nullptr;
}

void FObjectElementNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Object);
}

FString FObjectElementNode::GetReferencerName() const
{
	return TEXT("ObjectElementNode");
}

FElementNode::FResult FObjectElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	if (!ensure(Struct->IsA<UClass>()))
	{
		return FResult::Failure().Error(FText::FromString(TEXT("ObjectElementNode: struct type must be a UClass when creating an object.")));
	}
	auto Class = CastChecked<UClass>(Struct);
	Object = NewObject<UObject>(Outer, Class);
	return FResult::Success();
}

FElementNode::FResult FObjectElementNode::OnEnd()
{
	return FResult::Success();
}

FElementNode::FResult FObjectElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	return FResult::Success();
}

bool FObjectElementNode::HasProperty(const FStringView& AttributeName)
{
	if (!Object)
	{
		return false;
	}
	return FPropertyChainHandle::Create(Object, AttributeName).IsValid();
}

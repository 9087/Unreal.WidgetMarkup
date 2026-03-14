// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ObjectElementNode.h"

#include "PropertyChainHandle.h"
#include "PropertyElementNode.h"

UObject* FObjectElementNode::GetObject() const
{
	return Object;
}

void FObjectElementNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FElementNode::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(Object);
}

FElementNode::FResult FObjectElementNode::Begin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	if (!ensure(Struct->IsA<UClass>()))
	{
		return FResult::Failure().Error(FText::FromString(TEXT("ObjectElementNode: struct type must be a UClass when creating an object.")));
	}
	auto Class = CastChecked<UClass>(Struct);
	Object = NewObject<UObject>(Outer, Class);
	return FResult::Success();
}

FElementNode::FResult FObjectElementNode::End()
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

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "BasicTypeElementNode.h"
#include "ElementNodes/PropertyElementNode.h"
#include "ElementNodes/PropertyChainHandle.h"
#include "PropertyBuffer.h"
#include "UObject/UnrealType.h"

IMPLEMENT_ELEMENT_NODE(FBasicTypeElementNode, FElementNode)

FBasicTypeElementNode::FBasicTypeElementNode(const FStringView& InTypeName)
	: TypeName(InTypeName)
{
}

void FBasicTypeElementNode::SetElementData(const TCHAR* InElementData)
{
	if (InElementData)
	{
		ValueString = InElementData;
	}
}

FElementNode::FResult FBasicTypeElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* /*Struct*/)
{
	UObject* Object = Context.GetLastObjectNode() ? Context.GetLastObjectNode()->GetObject() : nullptr;
	if (!Object)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("BasicTypeElementNode: no object in context.")));
	}

	FWidgetPropertyPath ResolvedPath;
	FBufferedPropertyContext DummyBuffered;
	if (!FPropertyElementNode::TryResolveArrayElementPath(Context, INDEX_NONE, ResolvedPath, DummyBuffered))
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("BasicTypeElementNode: parent is not an array property.")),
			FText::FromString(TypeName)));
	}

	TSharedPtr<FPropertyChainHandle> Chain = FPropertyChainHandle::Create(Object, ResolvedPath);
	FProperty* TailProperty = Chain.IsValid() ? Chain->GetTailProperty() : nullptr;
	if (!TailProperty)
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("BasicTypeElementNode: cannot resolve tail property for path '{0}'.")),
			FText::FromString(ResolvedPath.GetPathName().ToString())));
	}

	ValueBuffer = MakeShared<FPropertyBuffer>(TailProperty, FStringView(ValueString));
	if (!ValueBuffer->HasValue())
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("BasicTypeElementNode: failed to convert '{0}' to type '{1}'.")),
			FText::FromString(ValueString), FText::FromString(TailProperty->GetClass()->GetName())));
	}

	return FResult::Success();
}

FElementNode::FResult FBasicTypeElementNode::OnEnd()
{
	ValueBuffer.Reset();
	return FResult::Success();
}

FElementNode::FResult FBasicTypeElementNode::OnAddChild(const TSharedRef<FElementNode>& /*Child*/)
{
	return FResult::Failure().Error(FText::FromString(TEXT("BasicTypeElementNode: leaf node cannot have children.")));
}

bool FBasicTypeElementNode::HasProperty(const FStringView& /*AttributeName*/)
{
	return false;
}

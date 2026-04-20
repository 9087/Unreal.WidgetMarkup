// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "StructElementNode.h"

#include "PropertyBuffer.h"
#include "PropertyElementNode.h"
#include "UObject/UnrealType.h"

IMPLEMENT_ELEMENT_NODE(FStructElementNode, FElementNode)

TSharedRef<FElementNode> FStructElementNode::Create()
{
	return MakeShared<FStructElementNode>();
}

FStructElementNode::~FStructElementNode()
{
	Cleanup();
}

void* FStructElementNode::GetStructMemory() const
{
	return StructPropertyBuffer.IsValid() ? StructPropertyBuffer->GetValueData() : nullptr;
}

UScriptStruct* FStructElementNode::GetScriptStruct() const
{
	return ScriptStruct;
}

const FBufferedPropertyContext& FStructElementNode::GetBufferedPropertyContext() const
{
	return StructBufferedPropertyContext;
}

UStruct* FStructElementNode::GetPropertyOwnerStruct() const
{
	return ScriptStruct;
}

FElementNode::FResult FStructElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	ScriptStruct = Cast<UScriptStruct>(Struct);
	if (!ScriptStruct)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("StructElementNode: Struct must be a UScriptStruct.")));
	}

	ValueProperty = new FStructProperty(FFieldVariant(), TEXT("Value"), RF_NoFlags);
	ValueProperty->Struct = ScriptStruct;
	ValueProperty->ElementSize = ScriptStruct->GetStructureSize();

	StructPropertyBuffer = MakeShared<FPropertyBuffer>(ValueProperty);
	if (!StructPropertyBuffer->HasValue())
	{
		Cleanup();
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("StructElementNode: failed to allocate memory for struct '{0}'.")),
			FText::FromString(ScriptStruct->GetName())));
	}

	const FWidgetPropertyPath RootPath(TEXT("Value"));
	StructBufferedPropertyContext = FBufferedPropertyContext(RootPath, StructPropertyBuffer);
	if (StructBufferedPropertyContext.InValid())
	{
		Cleanup();
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("StructElementNode: failed to create buffered property context for struct '{0}'.")),
			FText::FromString(ScriptStruct->GetName())));
	}

	return FResult::Success();
}

FElementNode::FResult FStructElementNode::OnEnd()
{
	return FResult::Success();
}

FElementNode::FResult FStructElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	if (CastElementNode<FPropertyElementNode>(Child.ToSharedPtr()))
	{
		return FResult::Success();
	}
	return FResult::Failure().Error(FText::Format(
		FText::FromString(TEXT("StructElementNode ({0}): only property child elements are supported.")),
		FText::FromString(ScriptStruct ? ScriptStruct->GetName() : TEXT("null"))));
}

bool FStructElementNode::HasProperty(const FStringView& AttributeName)
{
	if (!ScriptStruct)
	{
		return false;
	}
	return ScriptStruct->FindPropertyByName(FName(AttributeName)) != nullptr;
}

void FStructElementNode::Cleanup()
{
	StructBufferedPropertyContext.Reset();
	StructPropertyBuffer.Reset();
	if (ValueProperty)
	{
		delete ValueProperty;
		ValueProperty = nullptr;
	}
	ScriptStruct = nullptr;
}

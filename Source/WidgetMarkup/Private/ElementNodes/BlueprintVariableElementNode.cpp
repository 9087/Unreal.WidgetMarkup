// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "BlueprintVariableElementNode.h"

#include "WidgetMarkupBlueprintVariable.h"
#include "../Utilities/TypeParser.h"
#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"

IMPLEMENT_ELEMENT_NODE(FBlueprintVariableElementNode, FStructElementNode)

TSharedRef<FElementNode> FBlueprintVariableElementNode::Create()
{
	return MakeShared<FBlueprintVariableElementNode>();
}

FElementNode::FResult FBlueprintVariableElementNode::OnBegin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	FResult Result = FStructElementNode::OnBegin(Context, Outer, Struct);
	if (!Result)
	{
		return Result;
	}

	ParentBlueprint = Context.FindObject<UBlueprint>();
	if (!ParentBlueprint.IsValid())
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element must be nested under a Blueprint or WidgetBlueprint element.")));
	}
	return FResult::Success();
}

FElementNode::FResult FBlueprintVariableElementNode::OnEnd()
{
	UBlueprint* ParentBlueprintObject = ParentBlueprint.Get();
	if (!ParentBlueprintObject)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element has no owning Blueprint.")));
	}

	const auto* VariableData = static_cast<const FWidgetMarkupBlueprintVariable*>(GetStructMemory());
	if (!VariableData)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element has no struct data.")));
	}

	const FString VariableName = VariableData->Name.TrimStartAndEnd();
	const FString VariableType = VariableData->Type.TrimStartAndEnd();
	const FString VariableDefaultValue = VariableData->Default;

	if (VariableName.IsEmpty())
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element requires a non-empty Name attribute.")));
	}
	if (VariableType.IsEmpty())
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element requires a non-empty Type attribute.")));
	}

	FEdGraphPinType PinType;
	FString ParseError;
	if (!FTypeParser::ParseType(VariableType, PinType, ParseError))
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Variable '{0}' has invalid Type '{1}': {2}")),
			FText::FromString(VariableName),
			FText::FromString(VariableType),
			FText::FromString(ParseError)));
	}

	if (!FBlueprintEditorUtils::AddMemberVariable(ParentBlueprintObject, FName(VariableName), PinType, VariableDefaultValue))
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Failed to add variable '{0}' to Blueprint '{1}'. Variable name may conflict with existing member or parent class.")),
			FText::FromString(VariableName),
			FText::FromString(ParentBlueprintObject->GetName())));
	}

	return FStructElementNode::OnEnd();
}

FElementNode::FResult FBlueprintVariableElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	return FResult::Failure().Error(FText::FromString(TEXT("Variable element does not support child elements.")));
}

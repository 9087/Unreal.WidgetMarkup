// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "BlueprintVariableElementNode.h"

#include "../Utilities/TypeParser.h"
#include "Engine/Blueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"

IMPLEMENT_ELEMENT_NODE(FBlueprintVariableElementNode, FElementNode)

TSharedRef<FElementNode> FBlueprintVariableElementNode::Create()
{
	return MakeShared<FBlueprintVariableElementNode>();
}

FElementNode::FResult FBlueprintVariableElementNode::ApplyVariableAttribute(const FStringView& InAttributeName, const FStringView& InAttributeValue)
{
	const FString AttributeName(InAttributeName);
	const FString AttributeValue = FString(InAttributeValue).TrimStartAndEnd();

	if (AttributeName.Equals(TEXT("Name"), ESearchCase::IgnoreCase))
	{
		VariableName = AttributeValue;
		bNameSet = true;
		return FResult::Success();
	}
	if (AttributeName.Equals(TEXT("Type"), ESearchCase::IgnoreCase))
	{
		VariableType = AttributeValue;
		bTypeSet = true;
		return FResult::Success();
	}
	if (AttributeName.Equals(TEXT("Default"), ESearchCase::IgnoreCase))
	{
		VariableDefaultValue = AttributeValue;
		return FResult::Success();
	}

	return FResult::Failure().Error(FText::Format(
		FText::FromString(TEXT("Variable element: unsupported attribute '{0}'.")),
		FText::FromString(AttributeName)));
}

FElementNode::FResult FBlueprintVariableElementNode::Begin(const FContext& Context, UObject* Outer, UStruct* Struct)
{
	ParentBlueprint = Context.FindObject<UBlueprint>();
	if (!ParentBlueprint)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element must be nested under a Blueprint or WidgetBlueprint element.")));
	}
	return FResult::Success();
}

FElementNode::FResult FBlueprintVariableElementNode::End()
{
	if (!ParentBlueprint)
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element has no owning Blueprint.")));
	}
	if (!bNameSet || VariableName.TrimStartAndEnd().IsEmpty())
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element requires a non-empty Name attribute.")));
	}
	if (!bTypeSet || VariableType.TrimStartAndEnd().IsEmpty())
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

	if (!FBlueprintEditorUtils::AddMemberVariable(ParentBlueprint, FName(VariableName), PinType, VariableDefaultValue))
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Failed to add variable '{0}' to Blueprint '{1}'. Variable name may conflict with existing member or parent class.")),
			FText::FromString(VariableName),
			FText::FromString(ParentBlueprint->GetName())));
	}

	return FResult::Success();
}

FElementNode::FResult FBlueprintVariableElementNode::OnAddChild(const TSharedRef<FElementNode>& Child)
{
	return FResult::Failure().Error(FText::FromString(TEXT("Variable element does not support child elements.")));
}

bool FBlueprintVariableElementNode::HasProperty(const FStringView& AttributeName)
{
	return false;
}

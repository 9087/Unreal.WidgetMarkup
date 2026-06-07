// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "BlueprintVariableElementNode.h"

#include "BasicTypeElementNode.h"
#include "ObjectElementNode.h"
#include "PropertyBuffer.h"
#include "StructElementNode.h"
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
	FString VariableDefaultValue = VariableData->Default;

	if (VariableName.IsEmpty())
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element requires a non-empty Name attribute.")));
	}
	if (VariableType.IsEmpty())
	{
		return FResult::Failure().Error(FText::FromString(TEXT("Variable element requires a non-empty Type attribute.")));
	}

	// Parse container type info for validation.
	FString ContainerName, InnerTypeText, ParseError;
	const bool bIsContainer = FTypeParser::ParseContainer(VariableType, ContainerName, InnerTypeText, ParseError);
	const bool bIsBasicType = bIsContainer && FTypeParser::ToPinCategory(InnerTypeText) != NAME_None;
	const bool bIsStructType = bIsContainer && !bIsBasicType;

	// Ban string Default for container types with non-basic inner types
	// (e.g. Array<MyDataAsset>).  They MUST use child elements.
	if (bIsStructType && !VariableDefaultValue.IsEmpty())
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Variable '{0}' is a container of non-basic type. Use child elements instead of the Default attribute.")),
			FText::FromString(VariableName)));
	}

	// Collect child element values.
	if (DefaultValueChildren.Num() > 0)
	{
		TArray<FString> ChildValues;
		for (const TSharedRef<FElementNode>& Child : DefaultValueChildren)
		{
			FString ExportedValue;

			if (auto BasicNode = CastElementNode<FBasicTypeElementNode>(TSharedPtr<FElementNode>(Child)))
			{
				const TSharedPtr<const FPropertyBuffer> Buffer = BasicNode->GetValueBuffer();
				if (!Buffer.IsValid() || !Buffer->GetValueData())
				{
					return FResult::Failure().Error(FText::Format(
						FText::FromString(TEXT("Variable '{0}': child element has no value.")),
						FText::FromString(VariableName)));
				}
				Buffer->GetProperty()->ExportTextItem_Direct(ExportedValue, Buffer->GetValueData(), nullptr, nullptr, PPF_None);
			}
			else if (auto StructNode = CastElementNode<FStructElementNode>(TSharedPtr<FElementNode>(Child)))
			{
				UScriptStruct* StructType = StructNode->GetScriptStruct();
				void* StructMemory = StructNode->GetStructMemory();
				if (!StructType || !StructMemory)
				{
					return FResult::Failure().Error(FText::Format(
						FText::FromString(TEXT("Variable '{0}': struct child element has no data.")),
						FText::FromString(VariableName)));
				}
				StructType->ExportText(ExportedValue, StructMemory, nullptr, nullptr, PPF_None, nullptr);
			}
			else if (auto ObjectNode = CastElementNode<FObjectElementNode>(TSharedPtr<FElementNode>(Child)))
			{
				if (ObjectNode->IsPathReference())
				{
					ExportedValue = ObjectNode->GetObjectPath();
				}
				else
				{
					UObject* Object = ObjectNode->GetObject();
					if (!Object)
					{
						return FResult::Failure().Error(FText::Format(
							FText::FromString(TEXT("Variable '{0}': inline object child element has no object.")),
							FText::FromString(VariableName)));
					}
					// FObjectPropertyBase::ExportTextItem_Direct ≡ GetPathName().
					// These are semantically identical for object-type default values.
					Object->GetPathName(nullptr, ExportedValue);
				}
			}

			ChildValues.Add(ExportedValue);
		}

		const FString ChildDefault = FString::Join(ChildValues, TEXT(","));
		// Container types (Array/Set/Map) require parenthesized default values:
		//   Array: (Item1,Item2)  Set: (Item1,Item2)  Map: ((K1,V1),(K2,V2))
		const FString FormattedDefault = bIsContainer
			? FString::Printf(TEXT("(%s)"), *ChildDefault)
			: ChildDefault;
		if (!VariableDefaultValue.IsEmpty() && VariableDefaultValue != FormattedDefault)
		{
			return FResult::Failure().Error(FText::Format(
				FText::FromString(TEXT("Variable '{0}': Default attribute conflicts with child elements.")),
				FText::FromString(VariableName)));
		}
		VariableDefaultValue = FormattedDefault;
	}

	FEdGraphPinType PinType;
	FString PinTypeParseError;
	if (!FTypeParser::ParseType(VariableType, PinType, PinTypeParseError))
	{
		return FResult::Failure().Error(FText::Format(
			FText::FromString(TEXT("Variable '{0}' has invalid Type '{1}': {2}")),
			FText::FromString(VariableName),
			FText::FromString(VariableType),
			FText::FromString(PinTypeParseError)));
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
	if (CastElementNode<FBasicTypeElementNode>(TSharedPtr<FElementNode>(Child))
		|| CastElementNode<FStructElementNode>(TSharedPtr<FElementNode>(Child))
		|| CastElementNode<FObjectElementNode>(TSharedPtr<FElementNode>(Child)))
	{
		DefaultValueChildren.Add(Child);
		return FResult::Success();
	}
	return FResult::Failure().Error(FText::FromString(TEXT("Variable child elements must be basic-type literals, structs, or objects.")));
}

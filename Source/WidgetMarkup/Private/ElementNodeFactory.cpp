// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ElementNodeFactory.h"

#include "EdGraphSchema_K2.h"
#include "ElementNode.h"
#include "ElementNodes/BasicTypeElementNode.h"
#include "ElementNodes/StructElementNode.h"
#include "Utilities/TypeParser.h"

FElementNodeFactory& FElementNodeFactory::Get()
{
	static FElementNodeFactory ElementNodeFactory;
	return ElementNodeFactory;
}

bool FElementNodeFactory::Register(UStruct* Struct, const FOnCreateElementNode& OnCreateElementNode, const FRegisterOptions& RegisterOptions)
{
	if (!ensure(Struct) || !ensure(OnCreateElementNode.IsBound()))
	{
		return false;
	}
	CreateElementNodeDelegateMap.FindOrAdd(Struct) = OnCreateElementNode;

	if (RegisterOptions.Alias.IsSet())
	{
		FString Alias = RegisterOptions.Alias.GetValue().TrimStartAndEnd();
		if (!Alias.IsEmpty())
		{
			AliasToStructMap.FindOrAdd(Alias) = Struct;
		}
	}

	return true;
}

bool FElementNodeFactory::Unregister(UStruct* Struct)
{
	CreateElementNodeDelegateMap.Remove(Struct);

	TArray<FString> AliasesToRemove;
	for (const auto& Pair : AliasToStructMap)
	{
		if (Pair.Value.Get() == Struct)
		{
			AliasesToRemove.Add(Pair.Key);
		}
	}
	for (const FString& Alias : AliasesToRemove)
	{
		AliasToStructMap.Remove(Alias);
	}

	return true;
}

UStruct* FElementNodeFactory::ResolveStructByAlias(const FString& ElementName)
{
	if (const TWeakObjectPtr<UStruct>* AliasStruct = AliasToStructMap.Find(ElementName))
	{
		if (AliasStruct->IsValid())
		{
			return AliasStruct->Get();
		}

		AliasToStructMap.Remove(ElementName);
	}

	return nullptr;
}

TSharedPtr<FElementNode> FElementNodeFactory::CreateElementNode(UObject* Outer, const FString& ElementName, const TCHAR* ElementData, UStruct*& Struct)
{
	Struct = ResolveStructByAlias(ElementName);

	if (Struct == nullptr)
	{
		// Check basic types before falling through to TryFindTypeSlow,
		// which would log a warning for names like "float" or "int".
		// FTypeParser::ToPinCategory is shared with the Variable type system.
		const FName PinCategory = FTypeParser::ToPinCategory(ElementName);
		if (PinCategory != NAME_None)
		{
			// Struct types (Vector2D, LinearColor, etc.) must NOT be treated as
			// basic type leaf nodes — they need full StructElementNode handling
			// so their sub-properties (X, Y, etc.) can be set via child elements.
			if (PinCategory == UEdGraphSchema_K2::PC_Struct)
			{
				Struct = FTypeParser::ResolveStruct(ElementName);
			}
			else
			{
				auto Node = MakeShared<FBasicTypeElementNode>(FStringView(ElementName));
				if (ElementData)
				{
					Node->SetElementData(ElementData);
				}
				return Node;
			}
		}

		if (Struct == nullptr)
		{
			Struct = UClass::TryFindTypeSlow<UStruct>(ElementName, EFindFirstObjectOptions::EnsureIfAmbiguous);
		}
	}

	if (Struct == nullptr)
	{
		return nullptr;
	}
	UStruct* Best = nullptr;
	const FOnCreateElementNode* OnCreateElementNodePtr = nullptr;
	for (const auto& KeyValuePair : CreateElementNodeDelegateMap)
	{
		if (KeyValuePair.Key.Get() == nullptr)
		{
			continue;
		}
		if (Struct->IsChildOf(KeyValuePair.Key.Get()))
		{
			if (!Best || KeyValuePair.Key.Get()->IsChildOf(Best))
			{
				Best = KeyValuePair.Key.Get();
				OnCreateElementNodePtr = &(KeyValuePair.Value);
			}
		}
	}
	if (!OnCreateElementNodePtr)
	{
		// No registered delegate matched — if the resolved type is a UScriptStruct,
		// create a generic StructElementNode (handles Vector2D, LinearColor, etc.).
		if (Struct->IsA<UScriptStruct>())
		{
			return FStructElementNode::Create();
		}
		return nullptr;
	}
	const FOnCreateElementNode& OnCreateElementNode = *OnCreateElementNodePtr;
	return OnCreateElementNode.Execute();
}
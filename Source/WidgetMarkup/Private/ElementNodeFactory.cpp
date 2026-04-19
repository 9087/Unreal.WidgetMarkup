// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ElementNodeFactory.h"

#include "ElementNode.h"

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

TSharedPtr<FElementNode> FElementNodeFactory::CreateElementNode(UObject* Outer, const FString& ElementName, UStruct*& Struct)
{
	Struct = ResolveStructByAlias(ElementName);

	if (Struct == nullptr)
	{
		Struct = UClass::TryFindTypeSlow<UStruct>(ElementName, EFindFirstObjectOptions::EnsureIfAmbiguous);
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
		return nullptr;
	}
	const FOnCreateElementNode& OnCreateElementNode = *OnCreateElementNodePtr;
	return OnCreateElementNode.Execute();
}
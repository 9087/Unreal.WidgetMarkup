// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ElementNodeFactory.h"

#include "ElementNode.h"

FElementNodeFactory& FElementNodeFactory::Get()
{
	static FElementNodeFactory ElementNodeFactory;
	return ElementNodeFactory;
}

bool FElementNodeFactory::Register(UStruct* Struct, const FOnCreateElementNode& OnCreateElementNode)
{
	if (!ensure(OnCreateElementNode.IsBound()))
	{
		return false;
	}
	CreateElementNodeDelegateMap.FindOrAdd(Struct) = OnCreateElementNode;
	return true;
}

bool FElementNodeFactory::Unregister(UStruct* Struct)
{
	CreateElementNodeDelegateMap.Remove(Struct);
	return true;
}

TSharedPtr<FElementNode> FElementNodeFactory::CreateElementNode(UObject* Outer, const FString& ElementName, UStruct*& Struct)
{
	Struct = UClass::TryFindTypeSlow<UStruct>(ElementName, EFindFirstObjectOptions::EnsureIfAmbiguous);
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
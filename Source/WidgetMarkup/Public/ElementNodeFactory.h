// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

class FElementNode;

class FElementNodeFactory
{
public:
	static FElementNodeFactory& Get();

	DECLARE_DELEGATE_RetVal(TSharedRef<FElementNode>, FOnCreateElementNode)

	bool Register(UStruct* Struct, const FOnCreateElementNode& OnCreateElementNode);
	bool Unregister(UStruct* Struct);

	template <typename T>
	bool Register(FOnCreateElementNode OnCreateElementNode)
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return Register(T::StaticClass(), OnCreateElementNode);
		}
		else
		{
			return Register(T::StaticStruct(), OnCreateElementNode);
		}
	}

	template <typename T>
	bool Unregister()
	{
		if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return Unregister(T::StaticClass());
		}
		else
		{
			return Unregister(T::StaticStruct());
		}
	}

	TSharedPtr<FElementNode> CreateElementNode(UObject* Outer, const FString& ElementName, UStruct*& Struct);

private:
	TMap<TWeakObjectPtr<UStruct>, FOnCreateElementNode> CreateElementNodeDelegateMap;
};

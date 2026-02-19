// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

class FPropertyChainHandle
{
public:
	static TSharedPtr<FPropertyChainHandle> Create(UObject* Object, const TArray<FString>& PropertyNames);
	static TSharedPtr<FPropertyChainHandle> Create(UObject* Object, const FStringView& PropertyPath);

	FProperty* GetTailProperty() const;
	void* GetTailContainer() const;

	bool SetValue(const void* Data) const;
	bool SetValue(const FStringView& ValueString) const;

	TSharedPtr<FPropertyChainHandle> GetChildHandle(const FStringView& ChildName) const;

private:
	TWeakObjectPtr<UObject> Object;
	TSharedPtr<FEditPropertyChain> PropertyChain;
};
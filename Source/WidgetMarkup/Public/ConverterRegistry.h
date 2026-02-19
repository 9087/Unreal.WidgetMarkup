// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#pragma once

class FConverter;

class FConverterRegistry
{
public:
	static FConverterRegistry& Get();

	DECLARE_DELEGATE_RetVal(TSharedRef<FConverter>, FOnCreateConverter)

	bool Register(FName PropertyTypeName, const FOnCreateConverter& OnCreateConverter);
	bool Unregister(FName PropertyTypeName);
	TSharedPtr<FConverter> Convert(const FProperty& Property, void* Data, const FStringView& String);

private:
	TMap<FName, FOnCreateConverter> CreateConverterDelegateMap;
};

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "BooleanConverter.h"

TSharedRef<FConverter> FBooleanConverter::Create()
{
	return MakeShared<FBooleanConverter>();
}

bool FBooleanConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FBoolProperty>());
	auto& BoolProperty = *CastField<FBoolProperty>(&Property);
	if (String.Equals(TEXT("true"), ESearchCase::CaseSensitive) ||
		String.Equals(TEXT("True"), ESearchCase::CaseSensitive))
	{
		BoolProperty.SetPropertyValue(Data, true);
	}
	else if (
		String.Equals(TEXT("false"), ESearchCase::CaseSensitive) || 
		String.Equals(TEXT("False"), ESearchCase::CaseSensitive)
	)
	{
		BoolProperty.SetPropertyValue(Data, false);
	}
	else
	{
		return false;
	}
	return true;
}

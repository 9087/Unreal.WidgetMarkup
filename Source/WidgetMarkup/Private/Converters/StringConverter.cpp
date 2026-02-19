// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "StringConverter.h"

TSharedRef<FConverter> FStringConverter::Create()
{
	return MakeShared<FStringConverter>();
}

bool FStringConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FStrProperty>());
	auto& StrProperty = *CastField<FStrProperty>(&Property);
	StrProperty.SetPropertyValue(Data, String.GetData());
	return true;
}

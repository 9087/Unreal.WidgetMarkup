// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "NameConverter.h"

TSharedRef<FConverter> FNameConverter::Create()
{
	return MakeShared<FNameConverter>();
}

bool FNameConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FNameProperty>());
	auto& NameProperty = *CastField<FNameProperty>(&Property);
	NameProperty.SetPropertyValue(Data, FName(String));
	return true;
}

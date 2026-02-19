// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "EnumConverter.h"

TSharedRef<FConverter> FEnumConverter::Create()
{
	return MakeShared<FEnumConverter>();
}

bool FEnumConverter::Convert(const FStringView& String, UEnum* Enum, int64& OutValue)
{
	if (!Enum)
	{
		return false;
	}
	OutValue = Enum->GetValueByNameString(FString(String), EGetByNameFlags::CaseSensitive);
	return OutValue != INDEX_NONE;
}

bool FEnumConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FEnumProperty>());
	auto EnumProperty = CastField<FEnumProperty>(&Property);
	if (!EnumProperty)
	{
		return false;
	}
	auto Enum = EnumProperty->GetEnum();
	int64 EnumValue = INDEX_NONE;
	if (!Convert(String, Enum, EnumValue))
	{
		return false;
	}
	EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(Data, EnumValue);
	return true;
}

// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "ColorConverter.h"

TSharedRef<FConverter> FColorConverter::Create()
{
	return MakeShared<FColorConverter>();
}

bool FColorConverter::Convert(const FStringView& String, FColor& OutValue)
{
	if (!String.StartsWith(TEXT('#')))
	{
		return false;
	}
	OutValue = FColor::FromHex(FString(String));
	return true;
}

bool FColorConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FStructProperty>());
	auto& StructProperty = *CastField<FStructProperty>(&Property);
	check(StructProperty.Struct == TBaseStructure<FColor>::Get());
	FColor Color;
	if (!Convert(String, Color))
	{
		return false;
	}
	Property.CopyCompleteValue(Data, &Color);
	return true;
}

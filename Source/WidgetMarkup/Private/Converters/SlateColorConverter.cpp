// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "SlateColorConverter.h"

#include "ColorConverter.h"
#include "EnumConverter.h"
#include "LinearColorConverter.h"

TSharedRef<FConverter> FSlateColorConverter::Create()
{
	return MakeShared<FSlateColorConverter>();
}

bool FSlateColorConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FStructProperty>());
	auto& StructProperty = *CastField<FStructProperty>(&Property);
	check(StructProperty.Struct == FSlateColor::StaticStruct());
	FSlateColor SlateColor;
	FColor Color;
	FLinearColor LinearColor;
	EStyleColor StyleColor;
	if (FColorConverter::Convert(String, Color))
	{
		SlateColor = FSlateColor(Color);
	}
	else if (FLinearColorConverter::Convert(String, LinearColor))
	{
		SlateColor = FSlateColor(LinearColor);
	}
	else if (FEnumConverter::Convert(String, StyleColor))
	{
		SlateColor = FSlateColor(StyleColor);
	}
	else
	{
		return false;
	}
	StructProperty.CopyCompleteValue(Data, &SlateColor);
	return true;
}
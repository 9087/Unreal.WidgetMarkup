// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "MarginConverter.h"

TSharedRef<FConverter> FMarginConverter::Create()
{
	return MakeShared<FMarginConverter>();
}

bool FMarginConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FStructProperty>());
	auto& StructProperty = *CastField<FStructProperty>(&Property);
	check(StructProperty.Struct == TBaseStructure<FMargin>::Get());
	TArray<FString> Margins;
	FString(String).ParseIntoArray(Margins, TEXT(","));
	auto NumMargins = Margins.Num();
	using ValueType = decltype(FMargin::Left);
	if (NumMargins == 1)
	{
		ValueType Uniform;
		if (!LexTryParseString(Uniform, *Margins[0]))
		{
			return false;
		}
		FMargin Margin(Uniform);
		Property.CopyCompleteValue(Data, &Margin);
	}
	else if (NumMargins == 2)
	{
		ValueType Horizontal;
		ValueType Vertical;
		if (!LexTryParseString(Horizontal, *Margins[0]) ||
			!LexTryParseString(Vertical, *Margins[1]))
		{
			return false;
		}
		FMargin Margin(Horizontal, Vertical);
		Property.CopyCompleteValue(Data, &Margin);
	}
	else if (NumMargins == 4)
	{
		ValueType Left;
		ValueType Top;
		ValueType Right;
		ValueType Bottom;
		if (!LexTryParseString(Left, *Margins[0]) ||
			!LexTryParseString(Top, *Margins[1]) ||
			!LexTryParseString(Right, *Margins[2]) ||
			!LexTryParseString(Bottom, *Margins[3]))
		{
			return false;
		}
		FMargin Margin(Left, Top, Right, Bottom);
		Property.CopyCompleteValue(Data, &Margin);
	}
	else
	{
		return false;
	}
	return true;
}

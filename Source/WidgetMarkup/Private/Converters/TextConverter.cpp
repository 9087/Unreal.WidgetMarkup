// Copyright 2025 Wu Zhiwei. All Rights Reserved.

#include "TextConverter.h"

TSharedRef<FConverter> FTextConverter::Create()
{
	return MakeShared<FTextConverter>();
}

bool FTextConverter::Convert(const FProperty& Property, void* Data, const FStringView& String)
{
	check(Property.IsA<FTextProperty>());
	auto& TextProperty = *CastField<FTextProperty>(&Property);
	auto Text = FText::FromString(FString(String.GetData()));
	TextProperty.SetPropertyValue(Data, Text);
	return true;
}
